import Metal
import QuartzCore
import simd

class Shader{
    var pipelineState: MTLRenderPipelineState
    var depthStencilState: MTLDepthStencilState?

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
    
    let shaderPath = Bundle.main.path(forResource: "ModernBoy", ofType: "metallib") ??
        Bundle.main.resourcePath.flatMap{ "\($0)/ModernBoy.metallib" } ??
        "./ModernBoy.metallib"
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
