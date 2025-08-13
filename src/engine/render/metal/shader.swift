import Foundation
import Metal
import QuartzCore
import simd

// Fragment shader constant
struct RimConstant {
    var rimColor: simd_float3
    var rimPower: Float
    var rimStrength: Float
}

class Shader {
    var pipelineState: MTLRenderPipelineState

    var rimColor = simd_float3(repeating: 0.8)
    var rimPower: Float = 1.0
    var rimStrength: Float = 0.0

    init(
        _ device: MTLDevice,
        _ vsFunc: MTLFunction, _ fsFunc: MTLFunction,
        vertexDescriptor: MTLVertexDescriptor?,
        useDepth: Bool = true
    ) {
        let pipelineDesc = MTLRenderPipelineDescriptor()
        pipelineDesc.vertexFunction = vsFunc
        pipelineDesc.fragmentFunction = fsFunc
        pipelineDesc.vertexDescriptor = vertexDescriptor
        pipelineDesc.colorAttachments[0].pixelFormat = .rgba8Unorm
        pipelineDesc.depthAttachmentPixelFormat = useDepth ? .depth32Float_stencil8 : .invalid
        pipelineState = try! device.makeRenderPipelineState(descriptor: pipelineDesc)
    }

    // Convenience initializer for mesh-style shaders (with standard vertex attributes)
    convenience init(
        _ device: MTLDevice,
        _ vsFunc: MTLFunction, _ fsFunc: MTLFunction,
        _ useDepth: Bool = true
    ) {
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
        self.init(device, vsFunc, fsFunc, vertexDescriptor: vertexDesc, useDepth: useDepth)
    }

    func bind(encoder: MTLRenderCommandEncoder?) {
        guard let encoder = encoder else { return }
        encoder.setRenderPipelineState(pipelineState)
        var rimConstant = RimConstant(
            rimColor: rimColor,
            rimPower: rimPower,
            rimStrength: rimStrength
        )
        encoder.setFragmentBytes(
            &rimConstant,
            length: MemoryLayout<RimConstant>.stride,
            index: 1
        )
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
    _ fsFuncNamePtr: UnsafeRawPointer?
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

    let shader = rctx.createShader(vsFuncName, fsFuncName)
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

@_cdecl("Shader_setRimPower")
public func Shader_setRimPower(
    _ ptr: UnsafeRawPointer?,
    _ rimPower: Float
) {
    autoreleasepool {
        guard let ptr = ptr else { return }
        let shader = Unmanaged<Shader>
            .fromOpaque(ptr).takeUnretainedValue()
        shader.rimPower = rimPower
    }
}
@_cdecl("Shader_setRimStrength")
public func Shader_setRimStrength(
    _ ptr: UnsafeRawPointer?,
    _ rimStrength: Float
) {
    autoreleasepool {
        guard let ptr = ptr else { return }
        let shader = Unmanaged<Shader>
            .fromOpaque(ptr).takeUnretainedValue()
        shader.rimStrength = rimStrength
    }
}
