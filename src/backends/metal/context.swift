import Metal
import MetalKit
import QuartzCore
import simd

struct Line {
    var from: simd_float3
    var to: simd_float3
    var color: simd_float4
};

struct ViewConstant{
    var viewMat: simd_float4x4
    var projMat: simd_float4x4
}
struct ModelConstant{
    let modelMat: simd_float4x4
    let normalMat: simd_float3x3
}

class RenderContext {
    let semaphore = DispatchSemaphore(value: 3)

    let layer: CAMetalLayer
    let commandQueue: MTLCommandQueue
    let sampler: MTLSamplerState
    var depthStencilState: MTLDepthStencilState?
    var dsTexture: MTLTexture?

    var pickingTexture: MTLTexture?
    var pickedID = -1

    // material per frame
    var renderPassDesc: MTLRenderPassDescriptor?
    var commandBuffer: MTLCommandBuffer?
    var renderEncoder: MTLRenderCommandEncoder?
    var drawable: CAMetalDrawable?

    var idCommandBuffer: MTLCommandBuffer?
    var idRenderEncoder: MTLRenderCommandEncoder?

    var shaderLib: MTLLibrary
    var idShader: Shader

    init(_ layer: CAMetalLayer, _ libPath: String) {
        self.layer = layer
        layer.device = MTLCreateSystemDefaultDevice()!
        layer.pixelFormat = .bgra8Unorm
        commandQueue = layer.device!.makeCommandQueue()!

        let libURL = URL(fileURLWithPath: libPath)
        shaderLib = try! layer.device!.makeLibrary(URL: libURL)
        idShader = Shader(layer.device!,
            shaderLib.makeFunction(name: "vertex_main")!,
            shaderLib.makeFunction(name: "fragment_id")!
        )

        let dsd = MTLDepthStencilDescriptor()
        dsd.depthCompareFunction = .less
        dsd.isDepthWriteEnabled = true
        depthStencilState = layer.device!.makeDepthStencilState(descriptor: dsd)

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
        let ptd = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .rgba8Unorm,
            width: Int(layer.drawableSize.width),
            height: Int(layer.drawableSize.height),
            mipmapped: false
        )
        ptd.usage = [.renderTarget, .shaderRead]
        pickingTexture = layer.device!.makeTexture(descriptor: ptd)
    }
    deinit {
        guard let encoder = renderEncoder,
              let idEncoder = idRenderEncoder
              else { return; }
        encoder.endEncoding()
        idEncoder.endEncoding()
    }

    func frameStart(
        _ r: Double, _ g: Double, _ b: Double, _ a: Double
    ) {
        semaphore.wait()

        guard let drawable
            = layer.nextDrawable() else { return }
        self.drawable = drawable

        // Color Pass
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
        renderEncoder!.setDepthStencilState(depthStencilState)
        renderEncoder?.setCullMode(.back)
        renderEncoder!.setFragmentSamplerState(sampler, index: 0)

        // ID pass for mouse picking
        let prpd = MTLRenderPassDescriptor()
        prpd.colorAttachments[0].texture = pickingTexture
        prpd.colorAttachments[0].loadAction = .clear
        prpd.colorAttachments[0].clearColor = MTLClearColor(
            red: 0, green: 0, blue: 0, alpha: 0)
        prpd.colorAttachments[0].storeAction = .store
        prpd.depthAttachment.texture = dsTexture
        prpd.depthAttachment.loadAction = .clear
        prpd.depthAttachment.storeAction = .dontCare
        prpd.depthAttachment.clearDepth = 1.0

        idCommandBuffer = commandQueue.makeCommandBuffer()
        idRenderEncoder = idCommandBuffer?
            .makeRenderCommandEncoder(descriptor: prpd)
        idRenderEncoder?.setDepthStencilState(depthStencilState)
        idRenderEncoder?.setCullMode(.back)

        // bind ID Shader
        idRenderEncoder?.setRenderPipelineState(idShader.pipelineState)

        commandBuffer?.addCompletedHandler { _ in
            self.semaphore.signal()
        }
    }
    func setView(_ viewPos: simd_float3, _ fov: Float, _ viewQuat: simd_float4) {
        let aspectRatio = Float(layer.bounds.width / layer.bounds.height)
        var viewPosition = viewPos
        renderEncoder!.setFragmentBytes(&viewPosition,
            length: MemoryLayout<simd_float3>.stride, 
            index: 0)
        let viewMat = viewMatrix(viewPos, viewQuat)

        let projMat = perspectiveMatrix(
            fov: fov, aspectRatio: aspectRatio, nearPlane: 0.1, farPlane: 100.0)
        var viewConstant = ViewConstant(
            viewMat: viewMat, projMat: projMat)
        renderEncoder!.setVertexBytes(&viewConstant,
            length: MemoryLayout<ViewConstant>.stride,
            index: 1)
        idRenderEncoder!.setVertexBytes(&viewConstant,
            length: MemoryLayout<ViewConstant>.stride,
            index: 1)
    }
    func setShader(_ shader: Shader) {
        shader.bind(encoder: renderEncoder)
    }
    func setTexture(_ texture: Texture) {
        texture.bind(encoder: renderEncoder)
    }
    func draw(_ modelMat: simd_float4x4, _ mesh: Mesh, _ alpha: Float, _ id: Int) {
        guard let encoder = self.renderEncoder,
              let idEncoder = self.idRenderEncoder
              else {return }
        var modelConstant = ModelConstant(
            modelMat: modelMat, normalMat: normal(modelMat))
        encoder.setVertexBytes(&modelConstant,
            length: MemoryLayout<ModelConstant>.stride, index: 2)
        idEncoder.setVertexBytes(&modelConstant,
            length: MemoryLayout<ModelConstant>.stride, index: 2)

        var a = alpha
        encoder.setFragmentBytes(&a,
            length: MemoryLayout<Float>.stride,
            index: 2)

        var myIdColor = simd_float4(Float(id)/255.0, 0, 0, 0);
        var pickedIDColor = simd_float4(Float(pickedID)/255.0, 0, 0, 0);
        encoder.setFragmentBytes(&myIdColor,
            length: MemoryLayout<simd_float4>.stride,
            index: 3)
        encoder.setFragmentBytes(&pickedIDColor,
            length: MemoryLayout<simd_float4>.stride,
            index: 4)
        idRenderEncoder?.setFragmentBytes(&myIdColor,
            length: MemoryLayout<simd_float4>.stride,
            index: 0)

        encoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)
        idEncoder.setVertexBuffer(mesh.vertexBuffer, offset: 0, index: 0)

        if let indexBuffer = mesh.indexBuffer,
           let numIndices = mesh.numIndices, numIndices > 0 {
            encoder.drawIndexedPrimitives(type: .triangle,
                indexCount: numIndices, indexType: .uint32,
                indexBuffer: indexBuffer, indexBufferOffset: 0
            )
            idEncoder.drawIndexedPrimitives(type: .triangle,
                indexCount: numIndices, indexType: .uint32,
                indexBuffer: indexBuffer, indexBufferOffset: 0
            )
        } else{
            encoder.drawPrimitives(type: .triangle, vertexStart: 0,
                vertexCount: mesh.numVertices)
            idEncoder.drawPrimitives(type: .triangle, vertexStart: 0,
                vertexCount: mesh.numVertices)
        }
    }
    func drawLines(_ lines: UnsafePointer<Line>, _ count: Int){
        guard let encoder = renderEncoder
              else { return }
        let buffer = layer.device?.makeBuffer(
            bytes: lines, length: MemoryLayout<Line>.stride * count)

        encoder.setVertexBuffer(buffer, offset: 0, index: 0)
        encoder.drawPrimitives(type: .line, vertexStart: 0,
            vertexCount: count * 2)
    }
    func frameEnd() {
        guard let encoder = self.renderEncoder,
              let commandBuffer = self.commandBuffer,
              let drawable = self.drawable,
              let idEncoder = self.idRenderEncoder
              else {return }
        encoder.endEncoding()
        commandBuffer.present(drawable)
        commandBuffer.commit()

        idEncoder.endEncoding()
        idCommandBuffer?.commit()

        self.renderEncoder = nil
        self.commandBuffer = nil
        self.drawable = nil
        self.renderPassDesc = nil

        self.idRenderEncoder = nil
        self.idCommandBuffer = nil
    }
    func getPickedID(_ x: Int, _ y: Int) -> Int {
        var pixelColor = [UInt8](repeating: 0, count: 4)
        let region = MTLRegionMake2D(x, y, 1, 1)
        pickingTexture?.getBytes(
            &pixelColor,
            bytesPerRow: 4,
            from: region,
            mipmapLevel: 0
        )
        pickedID = Int(pixelColor[0])
        return pickedID
    }

    func createShader(
        _ vsFuncName: String, _ fsFuncName: String
    ) -> Shader? {
        let vsFunc = shaderLib.makeFunction(name: vsFuncName)
        let fsFunc = shaderLib.makeFunction(name: fsFuncName)

        guard let dev = layer.device,
              let vsFn = vsFunc,
              let fsFn = fsFunc
              else { return nil }

        return Shader(dev, vsFn, fsFn)
    }
}

@_cdecl("createRenderContext")
public func createRenderContext(_ layerPtr: UnsafeRawPointer?,
    _ libPathPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let layerPtr = layerPtr,
          let libPathPtr = libPathPtr
          else { fatalError("Cannot initialize RenderContext") }
    let layer = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue()
    let cStr = libPathPtr.assumingMemoryBound(to: CChar.self)
    let providedPath = String(cString: cStr)
    let libPath: String
    if FileManager.default.fileExists(atPath: providedPath) {
        libPath = providedPath
    }
    else if let bundlePath = Bundle.main.path(forResource: "ModernBoy", ofType: "metallib") {
        libPath = bundlePath
    } else {
        libPath = "./asset/shader/ModernBoy.metallib"
    }

    let context = RenderContext(layer, libPath)
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
) {
    guard let rctxPtr = rctxPtr else { return }      
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()

    rctx.frameStart(r, g, b, a)
}
@_cdecl("RenderContext_setView")
public func RenderContext_setView(_ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float, _ fov: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float
    
) {
    guard let rctxPtr = rctxPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let viewPos = simd_float3(px, py, pz)
    let viewQuat = simd_float4(rx, ry, rz, w)

    rctx.setView(viewPos, fov, viewQuat)
}
@_cdecl("RenderContext_setShader")
public func RenderContext_setShader(_ rctxPtr: UnsafeRawPointer?,
    _ shaderPtr: UnsafeRawPointer?
) {
    guard let rctxPtr = rctxPtr,
          let shaderPtr = shaderPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let shader = Unmanaged<Shader>
        .fromOpaque(shaderPtr).takeUnretainedValue()

    rctx.setShader(shader)
}
@_cdecl("RenderContext_setTexture")
public func RenderContext_setTexture(_ rctxPtr: UnsafeRawPointer?,
    _ texPtr: UnsafeRawPointer?
) {
    guard let rctxPtr = rctxPtr,
          let texPtr = texPtr else { return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let shader = Unmanaged<Texture>
        .fromOpaque(texPtr).takeUnretainedValue()

    rctx.setTexture(shader)
}
@_cdecl("RenderContext_draw")
public func RenderContext_draw(_ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float,
    _ sx: Float, _ sy: Float, _ sz: Float,
    _ meshPtr: UnsafeRawPointer?, _ alpha: Float,
    _ id: Int
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

    rctx.draw(modelMat, mesh, alpha, id)
}
@_cdecl("RenderContext_drawLines")
public func RenderContext_drawLines(_ rctxPtr: UnsafeRawPointer?,
    _ linesPtr: UnsafeRawPointer?, _ count: Int
){
    guard let rctxPtr = rctxPtr,
          let linesPtr = linesPtr
          else{ return }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let lines = linesPtr.bindMemory(to: Line.self, capacity: count)
    rctx.drawLines(lines, count)
}
@_cdecl("RenderContext_draw_")
public func RenderContext_draw_(_ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float,
    _ sx: Float, _ sy: Float, _ sz: Float,
    _ meshPtr: UnsafeRawPointer?, _ alpha: Float,
    _ id: Int
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

    rctx.draw(modelMat, mesh, alpha, id)
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

@_cdecl("RenderContext_getPickedID")
public func RenderContext_getPickedID(_ rctxPtr: UnsafeRawPointer?,
    _ x: Int, _ y: Int
) -> Int {
    guard let rctxPtr = rctxPtr else {return 0 }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let pickedID = rctx.getPickedID(x, y)
    return pickedID
}