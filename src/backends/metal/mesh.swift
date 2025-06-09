import Metal
import QuartzCore
import simd

struct SmolVertex{
    var position: simd_float4
    var color: simd_float4
}

class Mesh{
    var vertexBuffer: MTLBuffer
    var numVertices: Int
    var indexBuffer: MTLBuffer?
    var numIndices: Int?

    init(device: MTLDevice, vertices: [SmolVertex],
        indices: [UInt16]? = nil
    ) {
        vertexBuffer = device.makeBuffer(
            bytes: vertices,
            length: vertices.count*MemoryLayout<SmolVertex>.stride)!
        numVertices = vertices.count
        if let indices = indices {
            indexBuffer = device.makeBuffer(
                bytes: indices,
                length: indices.count*MemoryLayout<UInt16>.stride,
            )
            numIndices = indices.count
        }
    }
}

@_cdecl("makeTriangle")
public func makeTriangle(layerPtr: UnsafeRawPointer?)
-> UnsafeRawPointer? {
    let vertices = [
        SmolVertex(
            position: simd_float4(0, 1, 0, 1),
            color: simd_float4(1, 0, 0, 1)),
        SmolVertex(position: simd_float4(-1, -1, 0, 1),
            color: simd_float4(0, 1, 0, 1)),
        SmolVertex(position: simd_float4(1, -1, 0, 1),
            color: simd_float4(0, 0, 1, 1)),
    ]
    guard let layerPtr = layerPtr
        else { return nil }
    let device = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue().device!

    let mesh = Mesh(device: device, vertices: vertices)
    return UnsafeRawPointer(Unmanaged.passRetained(mesh).toOpaque())
}
@_cdecl("destroyMesh")
public func destroyMesh(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Mesh>.fromOpaque(ptr).release()
    }
}
