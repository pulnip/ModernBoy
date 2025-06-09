import Metal
import QuartzCore

class RenderContext {
    let layer: CAMetalLayer
    let commandQueue: MTLCommandQueue

    // material per frame
    var commandBuffer: MTLCommandBuffer?
    var renderEncoder: MTLRenderCommandEncoder?
    var drawable: CAMetalDrawable?

    init(_ layerPtr: UnsafeRawPointer?) {
        guard let ptr = layerPtr else {
            fatalError("Cannot initialize RenderContext")
        }
        layer = Unmanaged<CAMetalLayer>.fromOpaque(ptr).takeUnretainedValue()
        layer.device = MTLCreateSystemDefaultDevice()!
        layer.pixelFormat = .bgra8Unorm
        commandQueue = layer.device!.makeCommandQueue()!
    }

    func handle(_ cmd: RenderCommand) {
        switch cmd {
        case .frameStart(let c):
            handleFrameStart(c)
        case .draw(let c):
            handleDraw(c)
        case .frameEnd(let c):
            handleFrameEnd(c)
        }
    }

    func handleFrameStart(_ cmd: FrameStartCommand) {
        guard let drawable = cmd.layer.nextDrawable() else { return }
        self.drawable = drawable
        let rpd = MTLRenderPassDescriptor()
        rpd.colorAttachments[0].texture = drawable.texture
        rpd.colorAttachments[0].loadAction = .clear
        rpd.colorAttachments[0].clearColor = MTLClearColor(red: 0, green: 0, blue: 0, alpha: 0)
        rpd.colorAttachments[0].storeAction = .store
        rpd.colorAttachments[0].texture = drawable.texture

        commandBuffer = commandQueue.makeCommandBuffer()
        renderEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: rpd)

        cmd.shader.bind(encoder: renderEncoder!)
    }
    func handleDraw(_ cmd: DrawCommand) {
        guard let encoder = self.renderEncoder else {return }
        encoder.setVertexBuffer(cmd.mesh.vertexBuffer, offset: 0, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0,
            vertexCount: cmd.mesh.numVertices)
    }
    func handleFrameEnd(_ cmd: FrameEndCommand) {
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


