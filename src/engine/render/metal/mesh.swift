import Metal
import MetalKit
import QuartzCore
import simd

struct Vertex {
    var position: simd_float3
    var normal: simd_float3
    var uv: simd_float2
    // var tangent: simd_float3
}

class Mesh {
    var vertexBuffer: MTLBuffer
    var numVertices: Int
    var indexBuffer: MTLBuffer?
    var numIndices: Int?

    init(
        _ device: MTLDevice, _ vertices: [Vertex],
        _ indices: [UInt32]? = nil
    ) {
        vertexBuffer = device.makeBuffer(
            bytes: vertices,
            length: vertices.count * MemoryLayout<Vertex>.stride,
            options: .storageModeShared)!
        numVertices = vertices.count
        if let indices = indices {
            indexBuffer = device.makeBuffer(
                bytes: indices,
                length: indices.count * MemoryLayout<UInt32>.stride,
            )
            numIndices = indices.count
        }
    }
}

@_cdecl("createMesh")
public func createMesh(
    _ rctxPtr: UnsafeRawPointer?,
    _ packedVertices: UnsafePointer<Float>, _ numVertices: Int32,
    _ indicesPtr: UnsafePointer<UInt32>?, _ numIndices: Int32,
) -> UnsafeRawPointer? {
    guard let rctxPtr = rctxPtr else {
        fatalError("Invalid Metel Layer Pointer")
    }
    let device = Unmanaged<RenderContext>
        .fromOpaque(rctxPtr).takeUnretainedValue().layer.device!

    var vertices: [Vertex] = []
    for i in 0..<numVertices {
        let base = Int(i * 8)
        let position = simd_float3(
            packedVertices[base + 0], packedVertices[base + 1],
            packedVertices[base + 2])
        let normal = simd_float3(
            packedVertices[base + 3], packedVertices[base + 4],
            packedVertices[base + 5])
        let uv = simd_float2(
            packedVertices[base + 6], packedVertices[base + 7])
        // let tangent = simd_float3(
        //     packedVertices[base + 8], packedVertices[base + 9],
        //     packedVertices[base + 10])
        vertices.append(
            Vertex(
                position: position, normal: normal,
                uv: uv))  //, tangent: tangent))
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
