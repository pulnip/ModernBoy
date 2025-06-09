import simd
import QuartzCore

enum RenderCommand {
    case frameStart(FrameStartCommand)
    case draw(DrawCommand)
    case frameEnd(FrameEndCommand)
}

struct FrameStartCommand {
    var layer: CAMetalLayer
    var clearColor: simd_float4?
    var shader: Shader
}

struct DrawCommand {
    var mesh: Mesh
}

struct FrameEndCommand{

}