import Metal
import MetalKit
import QuartzCore
import simd

struct Line {
    var from: simd_float3
    var to: simd_float3
    var color: simd_float4
}

struct Point {
    var position: simd_float3
    var color: simd_float4
}

struct ViewConstant {
    var viewMat: simd_float4x4
    var projMat: simd_float4x4
}
struct ModelConstant {
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
    var secondaryEncoders: [MTLRenderCommandEncoder] = []

    var idCommandBuffer: MTLCommandBuffer?
    var idRenderEncoder: MTLRenderCommandEncoder?

    var shaderLib: MTLLibrary
    var idShader: Shader
    var lineShader: Shader
    var pointShader: Shader
    var pointComputeShader: ComputeShader

    let nCopies = 100
    var numPoint: Int

    var inBuf: MTLBuffer
    var outBuf: MTLBuffer

    init(_ layer: CAMetalLayer, _ libPath: String) {
        self.layer = layer
        layer.device = MTLCreateSystemDefaultDevice()!
        layer.pixelFormat = .rgba8Unorm
        commandQueue = layer.device!.makeCommandQueue()!

        let libURL = URL(fileURLWithPath: libPath)
        shaderLib = try! layer.device!.makeLibrary(URL: libURL)

        let dsd = MTLDepthStencilDescriptor()
        dsd.depthCompareFunction = .less
        dsd.isDepthWriteEnabled = true
        depthStencilState = layer.device!.makeDepthStencilState(descriptor: dsd)

        idShader = Shader(
            device: layer.device!,
            vsFunc: shaderLib.makeFunction(name: "vertex_main")!,
            fsFunc: shaderLib.makeFunction(name: "fragment_id")!,
            depthState: depthStencilState
        )
        lineShader = Shader(
            device: layer.device!,
            vsFunc: shaderLib.makeFunction(name: "vertex_line")!,
            fsFunc: shaderLib.makeFunction(name: "fragment_line")!,
            depthState: nil
        )
        pointShader = Shader(
            device: layer.device!,
            vsFunc: shaderLib.makeFunction(name: "vertex_points")!,
            fsFunc: shaderLib.makeFunction(name: "fragment_points")!,
            vertexDesc: nil,
            depthState: nil
        )
        pointComputeShader = ComputeShader(
            layer.device!,
            shaderLib.makeFunction(name: "expand_points")!
        )

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

        let pts: [Point] = [
            Point(position: simd_float3(-1.0, 1.0, 0.0), color: simd_float4(0, 0, 0, 1)),
            Point(position: simd_float3(-0.5, 1.0, 0.0), color: simd_float4(1, 0, 0, 1)),
            Point(position: simd_float3(+0.0, 1.0, 0.0), color: simd_float4(0, 1, 0, 1)),
            Point(position: simd_float3(+0.5, 1.0, 0.0), color: simd_float4(0, 0, 1, 1)),
            Point(position: simd_float3(+1.0, 1.0, 0.0), color: simd_float4(1, 1, 1, 1)),
        ]
        numPoint = pts.count

        inBuf = layer.device!.makeBuffer(
            length: MemoryLayout<Point>.stride * numPoint,
            options: .storageModeShared)!
        let byteCount = pts.count * MemoryLayout<Point>.stride
        inBuf.contents().copyMemory(from: pts, byteCount: byteCount)
        outBuf = layer.device!.makeBuffer(
            length: MemoryLayout<Point>.stride * numPoint * nCopies,
            options: .storageModePrivate)!
    }

    func frameStart(
        _ r: Double, _ g: Double, _ b: Double, _ a: Double
    ) {
        semaphore.wait()

        guard let drawable = layer.nextDrawable() else { return }
        self.drawable = drawable
        ensureSurfaceTexturesMatchDrawableSize()

        // Color Pass
        let rpd = MTLRenderPassDescriptor()
        rpd.colorAttachments[0].texture = drawable.texture
        rpd.colorAttachments[0].loadAction = .clear
        rpd.colorAttachments[0].clearColor = MTLClearColor(
            red: r, green: g, blue: b, alpha: a)
        rpd.colorAttachments[0].storeAction = .store
        rpd.depthAttachment.texture = dsTexture
        rpd.depthAttachment.loadAction = .clear
        rpd.depthAttachment.storeAction = .dontCare
        rpd.depthAttachment.clearDepth = 1.0

        renderPassDesc = rpd
        commandBuffer = commandQueue.makeCommandBuffer()
        // renderEncoder = commandBuffer?
        //     .makeRenderCommandEncoder(descriptor: rpd)
        // renderEncoder!.setDepthStencilState(depthStencilState)
        // renderEncoder?.setCullMode(.back)
        // renderEncoder!.setFragmentSamplerState(sampler, index: 0)
        renderEncoder = nil

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
    }

    private func ensureMainEncoder() -> MTLRenderCommandEncoder? {
        if let enc = renderEncoder { return enc }
        guard let rpd = renderPassDesc,
            let cmd = commandBuffer
        else { return nil }
        let enc = cmd.makeRenderCommandEncoder(descriptor: rpd)
        enc?.setDepthStencilState(depthStencilState)
        enc?.setCullMode(.back)
        enc?.setFragmentSamplerState(sampler, index: 0)
        renderEncoder = enc
        return enc
    }

    func beginSecondaryPass(
        loadAction: MTLLoadAction = .load, clearColor: MTLClearColor? = nil,
        useDepth: Bool = false
    ) -> MTLRenderCommandEncoder? {
        guard let drawable = drawable,
            let commandBuffer = commandBuffer
        else { return nil }
        let rpd = MTLRenderPassDescriptor()
        rpd.colorAttachments[0].texture = drawable.texture
        rpd.colorAttachments[0].storeAction = .store
        rpd.colorAttachments[0].loadAction = loadAction
        if let clearColor = clearColor {
            rpd.colorAttachments[0].clearColor = clearColor
        }
        if useDepth, let dsTexture = dsTexture {
            rpd.depthAttachment.texture = dsTexture
            rpd.depthAttachment.loadAction = .load
            rpd.depthAttachment.storeAction = .dontCare
        }
        let encoder = commandBuffer.makeRenderCommandEncoder(descriptor: rpd)
        encoder?.setCullMode(.none)
        if let encoder = encoder {
            secondaryEncoders.append(encoder)
        }
        return encoder
    }

    var viewConstant: ViewConstant?

    func setView(_ viewPos: simd_float3, _ fov: Float, _ viewQuat: simd_float4) {
        let aspectRatio = Float(layer.drawableSize.width / layer.drawableSize.height)
        var viewPosition = viewPos
        if let encoder = ensureMainEncoder() {
            encoder.setFragmentBytes(
                &viewPosition,
                length: MemoryLayout<simd_float3>.stride,
                index: Binding.viewPosition)
        }
        let viewMat = viewMatrix(viewPos, viewQuat)

        let projMat = perspectiveMatrix(
            fov: fov, aspectRatio: aspectRatio, nearPlane: 0.1, farPlane: 100.0)
        viewConstant = ViewConstant(
            viewMat: viewMat, projMat: projMat)
        if var vc = viewConstant {
            if let encoder = ensureMainEncoder() {
                encoder.setVertexBytes(
                    &vc,
                    length: MemoryLayout<ViewConstant>.stride,
                    index: Binding.viewConstant)
            }
            idRenderEncoder?.setVertexBytes(
                &vc,
                length: MemoryLayout<ViewConstant>.stride,
                index: Binding.viewConstant)
        }
    }
    func setShader(_ shader: Shader) {
        if let encoder = ensureMainEncoder() {
            shader.bind(encoder: encoder)
        }
    }
    func setTexture(_ texture: Texture) {
        if let encoder = ensureMainEncoder() {
            texture.bind(encoder: encoder)
        }
    }
    func draw(
        _ modelMat: simd_float4x4, _ mesh: Mesh, _ alpha: Float,
        _ id: Int, _ useUV: Bool, _ color: simd_float4
    ) {
        guard let encoder = ensureMainEncoder() else { return }
        guard let idEncoder = idRenderEncoder
        else { return }
        var modelConstant = ModelConstant(
            modelMat: modelMat, normalMat: normal(modelMat))
        encoder.setVertexBytes(
            &modelConstant,
            length: MemoryLayout<ModelConstant>.stride, index: Binding.modelConstant)

        var a = alpha
        encoder.setFragmentBytes(
            &a,
            length: MemoryLayout<Float>.stride,
            index: Binding.debugAlpha)

        var myIdColor = simd_float4(Float(id) / 255.0, 0, 0, 0)
        var pickedIDColor = simd_float4(Float(pickedID) / 255.0, 0, 0, 0)
        var color = color

        encoder.setFragmentBytes(
            &myIdColor,
            length: MemoryLayout<simd_float4>.stride,
            index: Binding.myIDColor)
        encoder.setFragmentBytes(
            &pickedIDColor,
            length: MemoryLayout<simd_float4>.stride,
            index: Binding.pickedIDColor)
        encoder.setFragmentBytes(
            &color,
            length: MemoryLayout<simd_float4>.stride,
            index: Binding.debugColor)
        encoder.setVertexBuffer(
            mesh.vertexBuffer, offset: 0,
            index: Binding.vertexObjectBuffer)

        if !useUV {
            idEncoder.setVertexBytes(
                &modelConstant,
                length: MemoryLayout<ModelConstant>.stride,
                index: Binding.modelConstant)
            idRenderEncoder?.setFragmentBytes(
                &myIdColor,
                length: MemoryLayout<simd_float4>.stride,
                index: Binding.myIDColor)

            idEncoder.setVertexBuffer(
                mesh.vertexBuffer, offset: 0,
                index: Binding.vertexObjectBuffer)
        }

        if let indexBuffer = mesh.indexBuffer,
            let numIndices = mesh.numIndices, numIndices > 0
        {
            encoder.drawIndexedPrimitives(
                type: .triangle,
                indexCount: numIndices, indexType: .uint32,
                indexBuffer: indexBuffer, indexBufferOffset: 0
            )
            if !useUV {
                idEncoder.drawIndexedPrimitives(
                    type: .triangle,
                    indexCount: numIndices, indexType: .uint32,
                    indexBuffer: indexBuffer, indexBufferOffset: 0
                )
            }
        } else {
            encoder.drawPrimitives(
                type: .triangle, vertexStart: 0,
                vertexCount: mesh.numVertices)
            if !useUV {
                idEncoder.drawPrimitives(
                    type: .triangle, vertexStart: 0,
                    vertexCount: mesh.numVertices)
            }
        }
    }
    func frameEnd(_ lines: UnsafePointer<Line>?, _ count: Int) {
        guard let encoder = self.renderEncoder,
            let commandBuffer = self.commandBuffer,
            let drawable = self.drawable,
            let idEncoder = self.idRenderEncoder
        else { return }
        // End of Color pass
        encoder.endEncoding()

        // End of ID pass
        idEncoder.endEncoding()
        idCommandBuffer?.commit()

        // Debug Line pass
        if count > 0, let lines = lines {
            if let lineEncoder = beginSecondaryPass(loadAction: .load, useDepth: false) {
                lineEncoder.setCullMode(.none)
                lineEncoder.setRenderPipelineState(lineShader.pipelineState)
                lineEncoder.setVertexBytes(
                    &(viewConstant!),
                    length: MemoryLayout<ViewConstant>.stride, index: 1)
                let lineBuffer = layer.device?.makeBuffer(
                    bytes: lines, length: MemoryLayout<Line>.stride * count)
                lineEncoder.setVertexBuffer(lineBuffer, offset: 0, index: 0)
                lineEncoder.drawPrimitives(
                    type: .line,
                    vertexStart: 0, vertexCount: count * 2)
                lineEncoder.endEncoding()
            }
        }
        if numPoint > 0 {
            let cce = commandBuffer.makeComputeCommandEncoder()
            let tg = MTLSize(width: 1, height: 1, depth: 1)
            let grid = MTLSize(width: numPoint, height: 1, depth: 1)
            pointComputeShader.encode(
                cce!,
                {
                    encoder,
                    pipeline in
                    encoder.setBuffer(inBuf, offset: 0, index: 0)
                    encoder.setBuffer(outBuf, offset: 0, index: 1)
                    var numPointU32 = UInt32(numPoint)
                    var nCopiesU32 = UInt32(nCopies)
                    encoder.setBytes(&numPointU32, length: MemoryLayout<UInt32>.stride, index: 2)
                    encoder.setBytes(&nCopiesU32, length: MemoryLayout<UInt32>.stride, index: 3)
                    var dir = simd_float3(0, 1, 0)
                    var step: Float = 0.02
                    encoder.setBytes(&dir, length: MemoryLayout<simd_float3>.stride, index: 4)
                    encoder.setBytes(&step, length: MemoryLayout<Float>.stride, index: 5)
                }, grid: grid, tg: tg)

            if let re = beginSecondaryPass(loadAction: .load, useDepth: false) {
                pointShader.bind(encoder: re)
                re.setVertexBuffer(outBuf, offset: 0, index: 0)
                var mvp = viewConstant!.projMat * viewConstant!.viewMat
                re.setVertexBytes(&mvp, length: MemoryLayout<matrix_float4x4>.stride, index: 1)
                re.drawPrimitives(type: .point, vertexStart: 0, vertexCount: numPoint * nCopies)
                re.endEncoding()
            }
        }

        commandBuffer.present(drawable)
        commandBuffer.addCompletedHandler { [weak self] _ in
            self?.semaphore.signal()
        }
        commandBuffer.commit()

        self.renderEncoder = nil
        self.commandBuffer = nil
        self.drawable = nil
        self.renderPassDesc = nil

        secondaryEncoders.removeAll()
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
        vsFuncName: String, fsFuncName: String,
        depthState: MTLDepthStencilState?
    ) -> Shader? {
        let vsFunc = shaderLib.makeFunction(name: vsFuncName)
        let fsFunc = shaderLib.makeFunction(name: fsFuncName)

        guard let dev = layer.device,
            let vsFn = vsFunc,
            let fsFn = fsFunc
        else { return nil }

        return Shader(
            device: dev,
            vsFunc: vsFn, fsFunc: fsFn, depthState: depthState)
    }

    private func ensureSurfaceTexturesMatchDrawableSize() {
        guard let dev = layer.device else { return }
        let w = Int(layer.drawableSize.width)
        let h = Int(layer.drawableSize.height)

        // Recreate depth-stencil texture if the size does not match
        let needNewDS = (dsTexture == nil) || (dsTexture!.width != w) || (dsTexture!.height != h)
        if needNewDS {
            let dstd = MTLTextureDescriptor.texture2DDescriptor(
                pixelFormat: .depth32Float_stencil8,
                width: w,
                height: h,
                mipmapped: false
            )
            dstd.usage = [.renderTarget, .shaderRead]
            dsTexture = dev.makeTexture(descriptor: dstd)
        }

        // Recreate picking texture if the size does not match
        let needNewPicking =
            (pickingTexture == nil) || (pickingTexture!.width != w) || (pickingTexture!.height != h)
        if needNewPicking {
            let ptd = MTLTextureDescriptor.texture2DDescriptor(
                pixelFormat: .rgba8Unorm,
                width: w,
                height: h,
                mipmapped: false
            )
            ptd.usage = [.renderTarget, .shaderRead]
            pickingTexture = dev.makeTexture(descriptor: ptd)
        }
    }
}

@_cdecl("createRenderContext")
public func createRenderContext(
    _ layerPtr: UnsafeRawPointer?,
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
    } else if let bundlePath = Bundle.main.path(forResource: "ModernBoy", ofType: "metallib") {
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
public func RenderContext_frameStart(
    _ rctxPtr: UnsafeRawPointer?,
    _ r: Double, _ g: Double, _ b: Double, _ a: Double,
) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()

        rctx.frameStart(r, g, b, a)
    }
}
@_cdecl("RenderContext_setView")
public func RenderContext_setView(
    _ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float, _ fov: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float

) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let viewPos = simd_float3(px, py, pz)
        let viewQuat = simd_float4(rx, ry, rz, w)

        rctx.setView(viewPos, fov, viewQuat)
    }
}
@_cdecl("RenderContext_setShader")
public func RenderContext_setShader(
    _ rctxPtr: UnsafeRawPointer?,
    _ shaderPtr: UnsafeRawPointer?
) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr,
            let shaderPtr = shaderPtr
        else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let shader = Unmanaged<Shader>
            .fromOpaque(shaderPtr).takeUnretainedValue()

        rctx.setShader(shader)
    }
}
@_cdecl("RenderContext_setTexture")
public func RenderContext_setTexture(
    _ rctxPtr: UnsafeRawPointer?,
    _ texPtr: UnsafeRawPointer?
) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr,
            let texPtr = texPtr
        else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let texture = Unmanaged<Texture>
            .fromOpaque(texPtr).takeUnretainedValue()

        rctx.setTexture(texture)
    }
}
@_cdecl("RenderContext_draw")
public func RenderContext_draw(
    _ rctxPtr: UnsafeRawPointer?,
    _ px: Float, _ py: Float, _ pz: Float,
    _ rx: Float, _ ry: Float, _ rz: Float, _ w: Float,
    _ sx: Float, _ sy: Float, _ sz: Float,
    _ meshPtr: UnsafeRawPointer?, _ alpha: Float, _ id: Int,
    _ useUV: Bool, _ r: Float, _ g: Float, _ b: Float, _ a: Float
) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr,
            let meshPtr = meshPtr
        else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let mesh = Unmanaged<Mesh>
            .fromOpaque(meshPtr).takeUnretainedValue()

        var modelMat = matrix_identity_float4x4
        translate(&modelMat, px, py, pz)
        rotate(&modelMat, rx, ry, rz, w)
        scale(&modelMat, sx, sy, sz)

        rctx.draw(modelMat, mesh, alpha, id, useUV, simd_float4(r, g, b, a))
    }
}
@_cdecl("RenderContext_frameEnd")
public func RenderContext_frameEnd(
    _ rctxPtr: UnsafeRawPointer?,
    _ linesPtr: UnsafeRawPointer?, _ count: Int
) {
    autoreleasepool {
        guard let rctxPtr = rctxPtr
        else { return }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let lines = linesPtr?.bindMemory(to: Line.self, capacity: count)
        rctx.frameEnd(lines, count)
    }
}

@_cdecl("RenderContext_getDevice")
public func RenderContext_getDevice(
    _ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return nil }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        if let device = rctx.layer.device {
            return UnsafeRawPointer(Unmanaged.passUnretained(device).toOpaque())
        }
        return nil
    }
}
@_cdecl("RenderContext_getRenderPassDesc")
public func RenderContext_getRenderPassDesc(
    _ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return nil }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        if let desc = rctx.renderPassDesc {
            return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
        }
        return nil
    }
}
@_cdecl("RenderContext_getCommandBuffer")
public func RenderContext_getCommandBuffer(
    _ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return nil }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        if let desc = rctx.commandBuffer {
            return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
        }
        return nil
    }
}
@_cdecl("RenderContext_getRenderEncoder")
public func RenderContext_getRenderEncoder(
    _ rctxPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return nil }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        if let desc = rctx.renderEncoder {
            return UnsafeRawPointer(Unmanaged.passUnretained(desc).toOpaque())
        }
        return nil
    }
}

@_cdecl("RenderContext_getPickedID")
public func RenderContext_getPickedID(
    _ rctxPtr: UnsafeRawPointer?,
    _ x: Int, _ y: Int
) -> Int {
    autoreleasepool {
        guard let rctxPtr = rctxPtr else { return 0 }
        let rctx = Unmanaged<RenderContext>
            .fromOpaque(rctxPtr).takeUnretainedValue()
        let pickedID = rctx.getPickedID(x, y)
        return pickedID
    }
}
