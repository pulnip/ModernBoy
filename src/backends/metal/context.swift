import Metal
import QuartzCore

class RenderContext {
    let layer: CAMetalLayer
    let commandQueue: MTLCommandQueue
    let sampler: MTLSamplerState

    // material per frame
    var commandBuffer: MTLCommandBuffer?
    var renderEncoder: MTLRenderCommandEncoder?
    var drawable: CAMetalDrawable?

    init(_ layerPtr: UnsafeRawPointer?) {
        guard let layerPtr = layerPtr else {
            fatalError("Cannot initialize RenderContext")
        }
        layer = Unmanaged<CAMetalLayer>
            .fromOpaque(layerPtr).takeUnretainedValue()
        layer.device = MTLCreateSystemDefaultDevice()!
        layer.pixelFormat = .bgra8Unorm
        commandQueue = layer.device!.makeCommandQueue()!

        let desc = MTLSamplerDescriptor()
            desc.minFilter = .linear
            desc.magFilter = .linear
            desc.sAddressMode = .repeat
            desc.tAddressMode = .repeat
        sampler = layer.device!.makeSamplerState(
            descriptor: desc)!
    }
    deinit {
        if let encoder = self.renderEncoder {
            encoder.endEncoding()
        }
    }

    func frameStart(
        _ r: Double, _ g: Double, _ b: Double, _ a: Double,
        _ shader: Shader
    ) {
        guard let drawable
            = layer.nextDrawable() else { return }
        self.drawable = drawable
        let rpd = MTLRenderPassDescriptor()
        rpd.colorAttachments[0].texture = drawable.texture
        rpd.colorAttachments[0].loadAction = .clear
        rpd.colorAttachments[0].clearColor = MTLClearColor(
            red: r, green: g, blue: b, alpha: a)
        rpd.colorAttachments[0].storeAction = .store
        rpd.colorAttachments[0].texture = drawable.texture

        commandBuffer = commandQueue.makeCommandBuffer()
        renderEncoder = commandBuffer?
            .makeRenderCommandEncoder(descriptor: rpd)

        shader.bind(encoder: renderEncoder!)
    }
    func draw(_ mesh: Mesh) {
        guard let encoder
            = self.renderEncoder else {return }
        encoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)
        if let texture = mesh.texture {
            encoder.setFragmentTexture(texture, index: 0)
            encoder.setFragmentSamplerState(sampler, index: 0)
        }
        if let indexBuffer = mesh.indexBuffer,
           let numIndices = mesh.numIndices, numIndices > 0 {
            encoder.drawIndexedPrimitives(type: .triangle,
                indexCount: numIndices, indexType: .uint32,
                indexBuffer: indexBuffer, indexBufferOffset: 0
            )
        } else{
            encoder.drawPrimitives(type: .triangle, vertexStart: 0,
                vertexCount: mesh.numVertices)
        }
    }
    func frameEnd() {
        guard let encoder = self.renderEncoder,
              let commandBuffer = self.commandBuffer,
              let drawable = self.drawable else {return }
        encoder.endEncoding()
        commandBuffer.present(drawable)
        commandBuffer.commit()

        self.renderEncoder = nil
        self.commandBuffer = nil
        self.drawable = nil
    }
}

@_cdecl("createRenderContext")
public func createRenderContext(_ layerPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    let context = RenderContext(layerPtr)
    return UnsafeRawPointer(Unmanaged.passRetained(context).toOpaque())
}
@_cdecl("destroyRenderContext")
public func destroyRenderContext(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<RenderContext>.fromOpaque(ptr).release()
    }
}

@_cdecl("RenderContext_frameStart")
public func RenderContext_frameStart(_ rctxPtr: UnsafeRawPointer?,
    _ r: Double, _ g: Double, _ b: Double, _ a: Double,
    _ shaderPtr: UnsafeRawPointer?
) {
    guard let rctxPtr = rctxPtr,
          let shaderPtr = shaderPtr else { return }      
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let shader = Unmanaged<Shader>
        .fromOpaque(shaderPtr).takeUnretainedValue()

    rctx.frameStart(r, g, b, a, shader)
}
@_cdecl("RenderContext_draw")
public func RenderContext_draw(_ rctxPtr: UnsafeRawPointer?,
    _ meshPtr: UnsafeRawPointer?,
) {
    guard let rctxPtr = rctxPtr,
          let meshPtr = meshPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let mesh = Unmanaged<Mesh>
        .fromOpaque(meshPtr).takeUnretainedValue()

    rctx.draw(mesh)
}
@_cdecl("RenderContext_frameEnd")
public func RenderContext_frameEnd(_ rctxPtr: UnsafeRawPointer?) {
    guard let rctxPtr = rctxPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    rctx.frameEnd()
}
