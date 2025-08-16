import Foundation
import Metal
import QuartzCore
import simd

enum Binding {
    // vertex shader
    static let vertexObjectBuffer = 0
    static let vertexSceneBuffer = 1
    static let fragmentMaterialBuffer = 2
    static let viewConstant = 3
    static let modelConstant = 4

    // framgment shader
    static let viewPosition = 0
    static let myIDColor = 1
    static let pickedIDColor = 2
    static let debugColor = 3
    static let debugAlpha = 4

    // texture
    static let textureBaseColor = 0
    static let textureNormal = 1
    static let textureMR = 2
    static let textureEmissive = 3

    // sampler
    static let samplerCommon = 0
}

enum RenderShaderTag {
    case PBR
    case Unlit
}

class Shader {
    var pipelineState: MTLRenderPipelineState
    let depthStencilState: MTLDepthStencilState?

    init(
        device: MTLDevice,
        vsFunc: MTLFunction, fsFunc: MTLFunction,
        vertexDesc: MTLVertexDescriptor?,
        depthState: MTLDepthStencilState?
    ) {
        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vsFunc
        pipelineDesc.fragmentFunction = fsFunc
        pipelineDesc.vertexDescriptor = vertexDesc
        pipelineDesc.colorAttachments[0].pixelFormat = .rgba8Unorm
        pipelineDesc.depthAttachmentPixelFormat =
            depthState != nil ? .depth32Float_stencil8 : .invalid

        pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDesc)
        depthStencilState = depthState
    }
    convenience init(
        device: MTLDevice,
        vsFunc: MTLFunction, fsFunc: MTLFunction,
        depthState: MTLDepthStencilState?
    ) {
        let vertexDesc = MTLVertexDescriptor()
        // position
        vertexDesc.attributes[0].format = .float3
        vertexDesc.attributes[0].offset = 0
        vertexDesc.attributes[0].bufferIndex = 0
        // normal
        vertexDesc.attributes[1].format = .float3
        vertexDesc.attributes[1].offset = 12
        vertexDesc.attributes[1].bufferIndex = 0
        // texcoord
        vertexDesc.attributes[2].format = .float2
        vertexDesc.attributes[2].offset = 24
        vertexDesc.attributes[2].bufferIndex = 0
        // tangent
        vertexDesc.attributes[3].format = .float3
        vertexDesc.attributes[3].offset = 32
        vertexDesc.attributes[3].bufferIndex = 0
        vertexDesc.layouts[0].stride = MemoryLayout<Vertex>.stride
        vertexDesc.layouts[0].stepRate = 1
        vertexDesc.layouts[0].stepFunction = .perVertex

        self.init(
            device: device, vsFunc: vsFunc, fsFunc: fsFunc,
            vertexDesc: vertexDesc,
            depthState: depthState
        )
    }

    func bind(encoder: MTLRenderCommandEncoder) {
        encoder.setRenderPipelineState(pipelineState)
        if let dss = depthStencilState {
            encoder.setDepthStencilState(dss)
        }
    }
}

class ComputeShader {
    var pipelineState: MTLComputePipelineState

    init(
        _ device: MTLDevice, _ cpFunc: MTLFunction
    ) {
        let pipelineDesc = MTLComputePipelineDescriptor()
        pipelineDesc.computeFunction = cpFunc
        pipelineState = try! device.makeComputePipelineState(
            descriptor: pipelineDesc,
            options: [],
            reflection: nil
        )
    }
    func encode(
        _ encoder: MTLComputeCommandEncoder,
        _ bind: (MTLComputeCommandEncoder, MTLComputePipelineState) -> Void,
        grid: MTLSize, tg: MTLSize
    ) {
        encoder.setComputePipelineState(pipelineState)
        bind(encoder, pipelineState)
        encoder.dispatchThreads(grid, threadsPerThreadgroup: tg)
        encoder.endEncoding()
    }
}

@_cdecl("createShader")
public func createShader(
    _ rctxPtr: UnsafeRawPointer?,
    _ vsFuncNamePtr: UnsafeRawPointer?,
    _ fsFuncNamePtr: UnsafeRawPointer?,
    _ useDepth: Bool
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let vsFuncNamePtr = vsFuncNamePtr,
        let fsFuncNamePtr = fsFuncNamePtr
    else { return nil }
    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()

    let vsFuncNameCStr = vsFuncNamePtr.assumingMemoryBound(to: CChar.self)
    let vsFuncName = String(cString: vsFuncNameCStr)
    let fsFuncNameCStr = fsFuncNamePtr.assumingMemoryBound(to: CChar.self)
    let fsFuncName = String(cString: fsFuncNameCStr)

    let shader = rctx.createShader(
        vsFuncName: vsFuncName, fsFuncName: fsFuncName,
        depthState: useDepth ? rctx.depthStencilState : nil)
    return UnsafeRawPointer(
        Unmanaged
            .passRetained(shader!).toOpaque())
}
@_cdecl("destroyShader")
public func destroyShader(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Shader>.fromOpaque(ptr).release()
    }
}
