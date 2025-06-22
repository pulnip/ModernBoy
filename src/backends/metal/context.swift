import Metal
import MetalKit
import QuartzCore
import simd

struct ViewConstant{
    var viewMat: simd_float4x4
    var projMat: simd_float4x4
}
struct ModelConstant{
    let modelMat: simd_float4x4
    let normalMat: simd_float3x3
}

class RenderContext {
    let layer: CAMetalLayer
    let commandQueue: MTLCommandQueue
    let sampler: MTLSamplerState
    var dsTexture: MTLTexture?

    var fov: Float = toRadians(from: 45)
    var aspectRatio: Float = 1.0
    var viewPosition = simd_float3(repeating: 0)
    var viewMat = matrix_identity_float4x4

    // material per frame
    var renderPassDesc: MTLRenderPassDescriptor?
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
                let dstd = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .depth32Float_stencil8,
            width: Int(layer.drawableSize.width),
            height: Int(layer.drawableSize.height),
            mipmapped: false
        )
        dstd.usage = [.renderTarget, .shaderRead]
        dsTexture = layer.device!.makeTexture(descriptor: dstd)
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
        rpd.depthAttachment.texture = dsTexture
        rpd.depthAttachment.loadAction = .clear
        rpd.depthAttachment.storeAction = .dontCare
        rpd.depthAttachment.clearDepth = 1.0

        renderPassDesc = rpd
        commandBuffer = commandQueue.makeCommandBuffer()
        renderEncoder = commandBuffer?
            .makeRenderCommandEncoder(descriptor: rpd)

        shader.bind(encoder: renderEncoder)
        renderEncoder?.setDepthStencilState(shader.depthStencilState)

        aspectRatio = Float(layer.bounds.width / layer.bounds.height)
        let projMat = perspectiveMatrix(
            fov: fov, aspectRatio: aspectRatio, nearPlane: 0.1, farPlane: 100.0)
        var viewConstant = ViewConstant(
            viewMat: viewMat, projMat: projMat)
        renderEncoder!.setVertexBytes(&viewConstant,
            length: MemoryLayout<ViewConstant>.stride, index: 1)
    }
    func draw(_ modelMat: simd_float4x4, _ mesh: Mesh) {
        guard let encoder
            = self.renderEncoder else {return }
        var modelConstant = ModelConstant(
            modelMat: modelMat, normalMat: normal(modelMat))
        encoder.setVertexBytes(&modelConstant,
            length: MemoryLayout<ModelConstant>.stride, index: 2)

        encoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)
        if let texture = mesh.texture {
            encoder.setFragmentTexture(texture, index: 0)
            encoder.setFragmentSamplerState(sampler, index: 0)
        }
        encoder.setFragmentBytes(&viewPosition,
            length: MemoryLayout<simd_float3>.stride, index: 0)

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
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float,
    _ sx: Float, _ sy: Float, _ sz: Float,
    _ meshPtr: UnsafeRawPointer?,
) {
    guard let rctxPtr = rctxPtr,
          let meshPtr = meshPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let mesh = Unmanaged<Mesh>
        .fromOpaque(meshPtr).takeUnretainedValue()

    var modelMat = matrix_identity_float4x4
        translate(&modelMat, px, py, pz)
        rotate(&modelMat, rx, ry, rz, w)
        scale(&modelMat, sx, sy, sz)

    rctx.draw(modelMat, mesh)
}
@_cdecl("RenderContext_draw_")
public func RenderContext_draw_(_ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float,
    _ sx: Float, _ sy: Float, _ sz: Float,
    _ meshPtr: UnsafeRawPointer?,
) {
    guard let rctxPtr = rctxPtr,
          let meshPtr = meshPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let mesh = Unmanaged<Mesh>
        .fromOpaque(meshPtr).takeUnretainedValue()

    var modelMat = matrix_identity_float4x4
        translate(&modelMat, px, py, pz)
        rotate(&modelMat, rx, ry, rz)
        scale(&modelMat, sx, sy, sz)

    rctx.draw(modelMat, mesh)
}
@_cdecl("RenderContext_frameEnd")
public func RenderContext_frameEnd(_ rctxPtr: UnsafeRawPointer?) {
    guard let rctxPtr = rctxPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    rctx.frameEnd()
}

@_cdecl("RenderContext_getDevice")
public func RenderContext_getDevice(_ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr else {return nil }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    if let desc = rctx.layer.device {
        return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
    }
    return nil
}
@_cdecl("RenderContext_getRenderPassDesc")
public func RenderContext_getRenderPassDesc(_ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr else {return nil }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    if let desc = rctx.renderPassDesc {
        return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
    }
    return nil
}
@_cdecl("RenderContext_getCommandBuffer")
public func RenderContext_getCommandBuffer(_ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr else {return nil }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    if let desc = rctx.commandBuffer {
        return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
    }
    return nil
}
@_cdecl("RenderContext_getRenderEncoder")
public func RenderContext_getRenderEncoder(_ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr else {return nil }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    if let desc = rctx.renderEncoder {
        return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
    }
    return nil
}

@_cdecl("RenderContext_setView")
public func RenderContext_setView(_ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float
) {
    guard let rctxPtr = rctxPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let viewPos = simd_float3(px, py, pz)
    let viewQuat = simd_float4(rx, ry, rz, w)
    rctx.viewPosition = viewPos
    rctx.viewMat = viewMatrix(viewPos, viewQuat)
}

@_cdecl("RenderContext_setfov")
public func RenderContext_setfov(_ rctxPtr: UnsafeRawPointer?,
    _ fov: Float
) {
    guard let rctxPtr = rctxPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    rctx.fov = toRadians(from: fov)
}