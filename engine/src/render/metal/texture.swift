import Metal
import MetalKit

class Texture {
    var texture: MTLTexture?

    init(_ device: MTLDevice, _ texturePath: String?) {
        if let path = texturePath {
            let url = URL(fileURLWithPath: path)
            let loader: MTKTextureLoader = MTKTextureLoader(
                device: device)

            let options: [MTKTextureLoader.Option: Any] = [.SRGB: false]
            texture = try? loader.newTexture(
                URL: url,
                options: options)
        }
    }
    init(
        _ device: MTLDevice, _ pixels: UnsafeRawPointer,
        _ width: Int, _ height: Int
    ) {
        let texDesc = MTLTextureDescriptor.texture2DDescriptor(
            pixelFormat: .rgba8Unorm,
            width: width,
            height: height,
            mipmapped: false
        )
        texDesc.usage = [.shaderRead, .shaderWrite]

        texture = device.makeTexture(descriptor: texDesc)

        let region = MTLRegionMake2D(0, 0, width, height)
        texture?.replace(
            region: region,
            mipmapLevel: 0,
            withBytes: pixels,
            bytesPerRow: width * 4  // R8G8B8A8
        )
    }
    func bind(encoder: MTLRenderCommandEncoder?) {
        guard let encoder = encoder else { return }
        encoder.setFragmentTexture(texture, index: 0)
    }
}

@_cdecl("createTextureFromPath")
public func createTextureFromPath(
    _ rctxPtr: UnsafeRawPointer?,
    _ texturePath: UnsafePointer<CChar>?,
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr
    else { return nil }
    let device = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue().layer.device!

    var texPath: String? = nil
    if let texturePath = texturePath {
        texPath = String(cString: texturePath)
    }
    let texture = Texture(device, texPath)
    return UnsafeRawPointer(Unmanaged.passRetained(texture).toOpaque())
}
@_cdecl("createTextureFromPixel")
public func createTextureFromPixel(
    _ rctxPtr: UnsafeRawPointer?,
    _ pixels: UnsafePointer<UInt8>?,
    _ width: Int32, _ height: Int32
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr,
        let pixels = pixels
    else { return nil }
    let device = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue().layer.device!

    let texture = Texture(device, pixels, Int(width), Int(height))
    return UnsafeRawPointer(Unmanaged.passRetained(texture).toOpaque())
}
@_cdecl("destroyTexture")
public func destroyTexture(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Texture>.fromOpaque(ptr).release()
    }
}
