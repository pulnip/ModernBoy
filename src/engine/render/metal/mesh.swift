import Metal
import MetalKit
import QuartzCore
import simd

struct SmolVertex{
    var position: simd_float4
    var color: simd_float4
}
struct Vertex{
    var position: simd_float4
    var normal: simd_float4
    var uv: simd_float4
}

class Mesh{
    var vertexBuffer: MTLBuffer
    var numVertices: Int
    var indexBuffer: MTLBuffer?
    var numIndices: Int?

    init(_ device: MTLDevice, _ vertices: [Vertex],
        _ indices: [UInt32]? = nil
    ) {
        vertexBuffer = device.makeBuffer(
            bytes: vertices,
            length: vertices.count*MemoryLayout<Vertex>.stride,
            options: .storageModeShared)!
        numVertices = vertices.count
        if let indices = indices {
            indexBuffer = device.makeBuffer(
                bytes: indices,
                length: indices.count*MemoryLayout<UInt32>.stride,
            )
            numIndices = indices.count
        }
    }
}

@_cdecl("createMesh")
public func createMesh(layerPtr: UnsafeRawPointer?,
    packedVertices: UnsafePointer<Float>, numVertices: Int32,
    indicesPtr: UnsafePointer<UInt32>?, numIndices: Int32,
) -> UnsafeRawPointer? {
    guard let layerPtr = layerPtr else {
        fatalError("Invalid Metel Layer Pointer")
    }
    let device = Unmanaged<CAMetalLayer>
        .fromOpaque(layerPtr).takeUnretainedValue().device!

    var vertices: [Vertex] = []
    for i in 0..<numVertices{
        let base = Int(i * 8)
        let position = simd_float4(
            packedVertices[base+0], packedVertices[base+1],
            packedVertices[base+2],                      1)
        let normal = simd_float4(
            packedVertices[base+3], packedVertices[base+4],
            packedVertices[base+5],                      1)
        let uv = simd_float4(
            packedVertices[base+6], packedVertices[base+7],
                                 0,                      0)
        vertices.append(Vertex(
            position: position, normal: normal, uv: uv))
    }
    var indices: [UInt32]? = nil
    if let indicesPtr = indicesPtr, numIndices > 0 {
        let buffer = UnsafeBufferPointer(
            start: indicesPtr, count: Int(numIndices))
        indices = Array(buffer)
    }
    let mesh = Mesh(device, vertices, indices)
    return UnsafeRawPointer(Unmanaged.passRetained(mesh).toOpaque())
}
@_cdecl("destroyMesh")
public func destroyMesh(_ ptr: UnsafeRawPointer?) {
    if let ptr = ptr {
        Unmanaged<Mesh>.fromOpaque(ptr).release()
    }
}
