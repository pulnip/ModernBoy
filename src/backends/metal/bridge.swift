import Metal
import Foundation
import QuartzCore

var device: MTLDevice! = MTLCreateSystemDefaultDevice()
var commandQueue: MTLCommandQueue! = device.makeCommandQueue()
var vertexBuffer: MTLBuffer? = nil

var metalLayer: CAMetalLayer!
var renderPassDescriptor: MTLRenderPassDescriptor?
var commandBuffer: MTLCommandBuffer?
var renderEncoder: MTLRenderCommandEncoder?
var pipelineState: MTLRenderPipelineState!

@_cdecl("setupMetal")
public func setupMetal(_ layerPtr: UnsafeRawPointer?) {
    guard let ptr = layerPtr else { return }
    metalLayer = Unmanaged<CAMetalLayer>.fromOpaque(ptr).takeUnretainedValue()
    metalLayer.device = device
    metalLayer.pixelFormat = .bgra8Unorm

    // load shader
    let shaderPath = Bundle.main.path(forResource: "default", ofType: "metallib") ??
        Bundle.main.resourcePath.flatMap{ "\($0)/default.metallib" } ??
        "./default.metallib"
    let url = URL(fileURLWithPath: shaderPath)
    let library = try! device.makeLibrary(URL: url)
    let vertexFunc = library.makeFunction(name: "vertex_main")
    let fragFunc = library.makeFunction(name: "fragment_main")

    // vertex desc: pos(3f), color(3f)
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
    print("Metal pipeline state created")
}
@_cdecl("uploadVertices")
public func uploadVertices(ptr: UnsafePointer<Float>?, count: Int){
    guard let ptr = ptr else { return }

    // (float: x,y,z,r,g,b) per vertex
    let byteCount = count * 6 * MemoryLayout<Float>.size
    print("UPLOAD: count=\(count), byteCount=\(byteCount)")
    vertexBuffer = device.makeBuffer(
        bytes: ptr, length: byteCount, options: [])
    print("Vertex buffer uploaded! count:", count)
}

var currentDrawable: CAMetalDrawable?

@_cdecl("renderStart")
public func renderStart(){
    // print("renderStart");
    guard let drawable = metalLayer?.nextDrawable() else { return }
    currentDrawable = drawable

    let rpd = MTLRenderPassDescriptor()
    rpd.colorAttachments[0].texture = drawable.texture
    rpd.colorAttachments[0].loadAction = .clear
    rpd.colorAttachments[0].clearColor = MTLClearColor(red: 0, green: 0, blue: 0, alpha: 0)
    rpd.colorAttachments[0].storeAction = .store
    rpd.colorAttachments[0].texture = drawable.texture
    renderPassDescriptor = rpd

    commandBuffer = commandQueue.makeCommandBuffer()
    renderEncoder = commandBuffer?.makeRenderCommandEncoder(descriptor: rpd)
    renderEncoder?.setRenderPipelineState(pipelineState)
}

@_cdecl("draw")
public func draw(){
    // print("draw");
    if let buffer = vertexBuffer, let encoder = renderEncoder{
        encoder.setVertexBuffer(buffer, offset: 0, index: 0)
        encoder.drawPrimitives(type: .triangle, vertexStart: 0, vertexCount: 3)
    }
}

@_cdecl("renderEnd")
public func renderEnd(){
    // print("renderEnd");
    renderEncoder?.endEncoding()
    if let cb = commandBuffer, let drawable = currentDrawable {
        cb.present(drawable)
        cb.commit()
    }
    renderEncoder = nil
    commandBuffer = nil
    currentDrawable = nil
}