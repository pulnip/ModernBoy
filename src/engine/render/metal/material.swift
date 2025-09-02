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
    func bind(encoder: MTLRenderCommandEncoder)
}

final class UnlitMaterial: Material {
    var baseColorMap: MTLTexture

    let samplerState: MTLSamplerState

    var materialConstants = MaterialConstants()

    init(
        _ baseColor: MTLTexture, _ sampler: MTLSamplerState,
    ) {
        baseColorMap = baseColor

        samplerState = sampler
    }
    func bind(encoder: MTLRenderCommandEncoder) {
        // encoder.setFragmentBytes(
        //     &materialConstants,
        //     length: MemoryLayout<MaterialConstants>.stride,
        //     index: Binding.fragmentMaterialBuffer)

        encoder.setFragmentTexture(baseColorMap, index: 0)

        encoder.setFragmentSamplerState(samplerState, index: Binding.samplerCommon)
    }
}

@_cdecl("createUnlitMaterialFromPath")
public func createUnlitMaterialFromPath(
    _ rctxPtr: UnsafeRawPointer?,
    _ baseColorFilePath: UnsafePointer<CChar>?,
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let baseColorFilePath = baseColorFilePath
    else { return nil }

    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let baseColorPath = String(cString: baseColorFilePath)
    let baseColorURL = URL(fileURLWithPath: baseColorPath)
    let loader = MTKTextureLoader(device: rctx.layer.device!)

    let options: [MTKTextureLoader.Option: Any] = [.SRGB: true]
    let baseColor = try? loader.newTexture(
        URL: baseColorURL, options: options
    )

    let material = UnlitMaterial(
        baseColor!, rctx.sampler)
    return UnsafeRawPointer(Unmanaged.passRetained(material).toOpaque())
}
@_cdecl("createUnlitMaterialFromPixel")
public func createUnlitMaterialFromPixel(
    _ rctxPtr: UnsafeRawPointer?,
    _ pixels: UnsafePointer<UInt8>?,
    _ width: Int32, _ height: Int32
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let pixels = pixels
    else { return nil }

    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let device = rctx.layer.device!

    let texDesc = MTLTextureDescriptor.texture2DDescriptor(
        pixelFormat: .rgba8Unorm,
        width: Int(width),
        height: Int(height),
        mipmapped: false
    )
    texDesc.usage = [.shaderRead, .shaderWrite]

    let baseColor = device.makeTexture(descriptor: texDesc)
    let region = MTLRegionMake2D(0, 0, Int(width), Int(height))
    baseColor?.replace(
        region: region,
        mipmapLevel: 0,
        withBytes: pixels,
        bytesPerRow: Int(width) * 4  // R8G8B8A8
    )

    let material = UnlitMaterial(
        baseColor!, rctx.sampler)
    return UnsafeRawPointer(Unmanaged.passRetained(material).toOpaque())
}

private func pixelFormatASTC(block: Int, srgb: Bool) -> MTLPixelFormat {
    switch block {
    case 4:
        return srgb ? .astc_4x4_srgb : .astc_4x4_ldr
    case 5:
        return srgb ? .astc_5x5_srgb : .astc_5x5_ldr
    case 6:
        return srgb ? .astc_6x6_srgb : .astc_6x6_ldr
    case 8:
        return srgb ? .astc_8x8_srgb : .astc_8x8_ldr
    default:
        // fallback
        return srgb ? .astc_6x6_srgb : .astc_6x6_ldr
    }
}

private func makeMetalTextureFromASTC(
    device: MTLDevice,
    gpuPayload: GpuPayload,
    srgb: Bool
) -> MTLTexture? {
    guard gpuPayload.kind == PAYLOAD_ASTC
    else { return nil }

    let w0 = Int(gpuPayload.width)
    let h0 = Int(gpuPayload.height)
    let levels = Int(gpuPayload.levels)
    let fmt = pixelFormatASTC(
        block: Int(gpuPayload.block), srgb: srgb)
    let desc = MTLTextureDescriptor.texture2DDescriptor(
        pixelFormat: fmt, width: w0, height: h0, mipmapped: levels > 1)
    desc.usage = [.shaderRead]

    guard let tex = device.makeTexture(descriptor: desc)
    else { return nil }

    let block = max(4, min(8, Int(gpuPayload.block)))
    let blockBytes = 16
    for lvl in 0..<levels {
        let w = max(1, w0 >> lvl)
        let h = max(1, h0 >> lvl)
        let rowBytes = ((w + block - 1) / block) * blockBytes
        let off = Int(gpuPayload.levelOffsets![lvl])
        let ptr = UnsafeRawPointer(gpuPayload.data!).advanced(by: off)
        let region = MTLRegionMake2D(0, 0, w, h)
        tex.replace(region: region, mipmapLevel: lvl, withBytes: ptr, bytesPerRow: rowBytes)
    }
    return tex
}

@_cdecl("createUnlitMaterialFromASTC")
public func createUnlitMaterialFromASTC(
    _ rctxPtr: UnsafeRawPointer?,
    _ baseColorASTC: UnsafePointer<GpuPayload>?
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let baseColorASTC = baseColorASTC
    else { return nil }

    let rctx = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue()
    let gpuPayload = baseColorASTC.pointee

    let baseColor = makeMetalTextureFromASTC(
        device: rctx.layer.device!,
        gpuPayload: gpuPayload,
        srgb: true)

    let material = UnlitMaterial(
        baseColor!, rctx.sampler)
    return UnsafeRawPointer(Unmanaged.passRetained(material).toOpaque())
}

@_cdecl("destroyUnlitMaterial")
public func destroyUnlitMaterial(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<UnlitMaterial>.fromOpaque(ptr).release()
    }
}
