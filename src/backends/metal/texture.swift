import Metal;
import MetalKit;

class Texture{
    var texture: MTLTexture?

    init(_ device: MTLDevice, _ texturePath: String?){
        if let path = texturePath{
            let url = URL(fileURLWithPath: path)
            let loader: MTKTextureLoader = MTKTextureLoader(
                device: device)

            let options: [MTKTextureLoader.Option: Any] = [.SRGB: false]
            texture = try? loader.newTexture(URL: url,
                options: options)
        }
    }
    func bind(encoder: MTLRenderCommandEncoder?){
        guard let encoder = encoder else { return }
        encoder.setFragmentTexture(texture, index: 0)
    }
}

@_cdecl("createTexture")
public func createTexture(
    texturePath: UnsafePointer<CChar>?,
    layerPtr: UnsafeRawPointer?
) -> UnsafeRawPointer? {
    guard let layerPtr = layerPtr else {
        fatalError("Invalid Metel Layer Pointer")
    }
    let device = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue().device!

    var texPath: String? = nil
    if let texturePath = texturePath {
        texPath = String(cString: texturePath)
    }
    let texture = Texture(device, texPath)
    return UnsafeRawPointer(Unmanaged.passRetained(texture).toOpaque())
}
@_cdecl("destroyTexture")
public func destroyTexture(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Texture>.fromOpaque(ptr).release()
    }
}