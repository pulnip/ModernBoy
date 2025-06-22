import Metal
import QuartzCore
import simd

// Fragment shader constant
struct RimConstant{
    var rimColor: simd_float3
    var rimPower: Float
    var rimStrength: Float
}

class Shader{
    var pipelineState: MTLRenderPipelineState
    var depthStencilState: MTLDepthStencilState?

    var rimColor = simd_float3(repeating: 0.8)
    var rimPower: Float = 1.0
    var rimStrength: Float = 0.0

    init(device: MTLDevice, shaderPath: String) {
        let url = URL(fileURLWithPath: shaderPath)
        let lib = try! device.makeLibrary(URL: url)
        let vertexFunc = lib.makeFunction(name: "vertex_main")
        let fragFunc = lib.makeFunction(name: "fragment_main")

        let vertexDesc = MTLVertexDescriptor()
        vertexDesc.attributes[0].format = .float4
        vertexDesc.attributes[0].offset = 0
        vertexDesc.attributes[0].bufferIndex = 0
        vertexDesc.attributes[1].format = .float4
        vertexDesc.attributes[1].offset = 16
        vertexDesc.attributes[1].bufferIndex = 0
        vertexDesc.attributes[2].format = .float4
        vertexDesc.attributes[2].offset = 32
        vertexDesc.attributes[2].bufferIndex = 0
        vertexDesc.layouts[0].stride = MemoryLayout<Vertex>.stride
        vertexDesc.layouts[0].stepRate = 1
        vertexDesc.layouts[0].stepFunction = .perVertex

        let dsd = MTLDepthStencilDescriptor()
        dsd.depthCompareFunction = .less
        dsd.isDepthWriteEnabled = true
        depthStencilState = device.makeDepthStencilState(descriptor: dsd)

        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vertexFunc
        pipelineDesc.fragmentFunction = fragFunc
        pipelineDesc.vertexDescriptor = vertexDesc
        pipelineDesc.colorAttachments[0].pixelFormat = .bgra8Unorm
        pipelineDesc.depthAttachmentPixelFormat = .depth32Float_stencil8

        pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDesc)
    }
    func bind(encoder: MTLRenderCommandEncoder?){
        guard let encoder = encoder else { return }
        encoder.setRenderPipelineState(pipelineState)
        var rimConstant = RimConstant(
            rimColor: rimColor,
            rimPower: rimPower,
            rimStrength: rimStrength
        )
        encoder.setFragmentBytes(&rimConstant,
            length: MemoryLayout<RimConstant>.stride,
            index: 1
        )
    }
}

@_cdecl("createShader")
public func createShader(_ layerPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let layerPtr = layerPtr
        else { return nil }
    let device = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue().device!
    
    let shaderPath = Bundle.main.path(
        forResource: "ModernBoy", ofType: "metallib") ??
        "./asset/shader/ModernBoy.metallib"
    let shader = Shader(device: device,
    shaderPath: shaderPath)
    return UnsafeRawPointer(Unmanaged
        .passRetained(shader).toOpaque())
}
@_cdecl("destroyShader")
public func destroyShader(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Shader>.fromOpaque(ptr).release()
    }
}

@_cdecl("Shader_setRimPower")
public func Shader_setRimPower(_ ptr: UnsafeRawPointer?,
    _ rimPower: Float
) {
    guard let ptr = ptr else { return }
    let shader = Unmanaged<Shader>
        .fromOpaque(ptr).takeUnretainedValue()
    shader.rimPower = rimPower
}
@_cdecl("Shader_setRimStrength")
public func Shader_setRimStrength(_ ptr: UnsafeRawPointer?,
    _ rimStrength: Float
) {
    guard let ptr = ptr else { return }
    let shader = Unmanaged<Shader>
        .fromOpaque(ptr).takeUnretainedValue()
    shader.rimStrength = rimStrength
}
