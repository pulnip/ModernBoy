import Metal

class Shader{
    var pipelineState: MTLRenderPipelineState

    init(device: MTLDevice, shaderPath: String) {
        let url = URL(fileURLWithPath: shaderPath)
        let lib = try! device.makeLibrary(URL: url)
        let vertexFunc = lib.makeFunction(name: "vertex_main")
        let fragFunc = lib.makeFunction(name: "fragment_main")

        let vertexDesc = MTLVertexDescriptor()
        vertexDesc.attributes[0].format = .float3
        vertexDesc.attributes[0].offset = 0
        vertexDesc.attributes[0].bufferIndex = 0
        vertexDesc.attributes[1].format = .float3
        vertexDesc.attributes[1].offset = 3 * MemoryLayout<Float>.size
        vertexDesc.attributes[1].bufferIndex = 0
        vertexDesc.layouts[0].stride = 6 * MemoryLayout<Float>.size

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