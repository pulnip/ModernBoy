import Metal
import QuartzCore

class Shader{
    var pipelineState: MTLRenderPipelineState

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
        vertexDesc.layouts[0].stride = 32

        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vertexFunc
        pipelineDesc.fragmentFunction = fragFunc
        pipelineDesc.vertexDescriptor = vertexDesc
        pipelineDesc.colorAttachments[0].pixelFormat = .bgra8Unorm

        pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDesc)
    }
    func bind(encoder: MTLRenderCommandEncoder){
        encoder.setRenderPipelineState(pipelineState)
    }
}

@_cdecl("createShader")
public func createShader(_ layerPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let layerPtr = layerPtr
        else { return nil }
    let device = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue().device!
    let shaderPath = Bundle.main.path(forResource: "VS", ofType: "metallib") ??
        Bundle.main.resourcePath.flatMap{ "\($0)/VS.metallib" } ??
        "./VS.metallib"
    let context = Shader(device: device,
        shaderPath: shaderPath)
    return UnsafeRawPointer(Unmanaged.passRetained(context).toOpaque())
}
@_cdecl("destroyShader")
public func destroyShader(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Shader>.fromOpaque(ptr).release()
    }
}
