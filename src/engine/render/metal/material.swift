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
@_cdecl("destroyUnlitMaterial")
public func destroyUnlitMaterial(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<UnlitMaterial>.fromOpaque(ptr).release()
    }
}
