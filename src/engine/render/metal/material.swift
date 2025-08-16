import Foundation
import Metal
import MetalKit
import simd

struct MaterialConstants {
    var baseColorFactor: simd_float4 = [1, 1, 1, 1]
    var emissiveFactor: simd_float3 = [0, 0, 0]
    var metallic: Float = 1.0
    var roughness: Float = 1.0
    var flags: UInt32 = 0
}

protocol Material {
    var renderShader: Shader { get }

    func bind(encoder: MTLRenderCommandEncoder)
}

final class PBRMaterial: Material {
    let renderShader: Shader

    var baseColorMap: Texture
    var normalMap: Texture?
    var mrMap: Texture?
    var emissiveMap: Texture?

    let samplerState: MTLSamplerState

    var materialConstants = MaterialConstants()
    // var constantBuffer: MTLBuffer

    init(
        _ shader: Shader, _ baseColor: Texture, _ sampler: MTLSamplerState,
        normal: Texture?, mr: Texture?, emissive: Texture?
    ) {
        renderShader = shader
        baseColorMap = baseColor
        normalMap = normal
        mrMap = mr
        emissiveMap = emissive

        samplerState = sampler
    }
    func bind(encoder: MTLRenderCommandEncoder) {
        renderShader.bind(encoder: encoder)

        encoder.setFragmentBytes(
            &materialConstants,
            length: MemoryLayout<MaterialConstants>.stride,
            index: Binding.fragmentMaterialBuffer)

        baseColorMap.bind(encoder: encoder)
        if let t = normalMap { t.bind(encoder: encoder) }
        if let t = mrMap { t.bind(encoder: encoder) }
        if let t = emissiveMap { t.bind(encoder: encoder) }

        encoder.setFragmentSamplerState(samplerState, index: Binding.samplerCommon)
    }
}

@_cdecl("createPBRMaterial")
public func createPBRMaterial(
    _ rctxPtr: UnsafeRawPointer?,
    _ shaderPtr: UnsafeRawPointer?,
    _ baseColorPtr: UnsafeRawPointer?,
    _ normalPtr: UnsafeRawPointer?,
    _ mrPtr: UnsafeRawPointer?,
    _ emissivePtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let shaderPtr = shaderPtr,
        let baseColorPtr = baseColorPtr
    else { return nil }

    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let shader = Unmanaged<Shader>
        .fromOpaque(shaderPtr).takeUnretainedValue()
    let baseColor = Unmanaged<Texture>
        .fromOpaque(baseColorPtr).takeUnretainedValue()

    var normalMap: Texture? = nil
    var mrMap: Texture? = nil
    var emissiveMap: Texture? = nil

    if let t = normalPtr {
        normalMap = Unmanaged<Texture>
            .fromOpaque(t).takeUnretainedValue()
    }
    if let t = mrPtr {
        mrMap = Unmanaged<Texture>
            .fromOpaque(t).takeUnretainedValue()
    }
    if let t = emissivePtr {
        emissiveMap = Unmanaged<Texture>
            .fromOpaque(t).takeUnretainedValue()
    }

    let material = PBRMaterial(
        shader, baseColor, rctx.sampler,
        normal: normalMap, mr: mrMap, emissive: emissiveMap)
    return UnsafeRawPointer(Unmanaged.passRetained(material).toOpaque())
}
@_cdecl("destroyPBRMaterial")
public func destroyMaterial(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<PBRMaterial>.fromOpaque(ptr).release()
    }
}
