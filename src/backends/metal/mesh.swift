import Metal
import QuartzCore
import simd

struct SmolVertex{
    var position: simd_float3
    var color: UInt32
}

struct Mesh{
    var vertexBuffer: MTLBuffer
    var numVertices: Int
    var indexBuffer: MTLBuffer?
    var numIndices: Int?

    init(device: MTLDevice, vertices: [SmolVertex], indices: [UInt16]?) {
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
    let verts: [Float] = [
         0.0,  1.0, 0.0,  1,0,0,
        -1.0, -1.0, 0.0,  0,1,0,
         1.0, -1.0, 0.0,  0,0,1,
    ]

    guard let ptr = layerPtr else { return nil }
    let device = Unmanaged<CAMetalLayer>.fromOpaque(ptr).takeUnretainedValue().device!

    // (float: x,y,z,r,g,b) per vertex
    let byteCount = 3*6*MemoryLayout<Float>.size
    let vertexBuffer = device.makeBuffer(
        bytes: verts, length: byteCount)
    if let buf = vertexBuffer {
        return UnsafeRawPointer(Unmanaged.passUnretained(buf).toOpaque())
    }
    return nil
}
