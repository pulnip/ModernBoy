import Foundation
import simd

func toRadians(from degree: Float) -> Float {
    return degree * .pi / 180.0
}
func toQuat(euler: simd_float3
) -> simd_float4 {
    let c = cos(euler * 0.5)
    let s = sin(euler * 0.5)

    var quat = simd_float4(repeating: 1.0)

    quat.w = c.x * c.y * c.z + s.x * s.y * s.z
    quat.x = s.x * c.y * c.z - c.x * s.y * s.z
    quat.y = c.x * s.y * c.z + s.x * c.y * s.z
    quat.z = c.x * c.y * s.z - s.x * s.y * c.z

    return quat
}

func translate(_ matrix: inout simd_float4x4,
    _ position: simd_float3
) {
    matrix[3] = matrix[0]*position.x + matrix[1]*position.y
        + matrix[2]*position.z + matrix[3]
}
func translate(_ matrix: inout simd_float4x4,
    _ x: Float, _ y: Float, _ z: Float
) { translate(&matrix, simd_float3(x, y, z)) }

func rotate(_ matrix: inout simd_float4x4,
    _ quaternion: simd_float4
) {
    let q = simd_quatf(vector: quaternion)
    let rotMat = simd_float4x4(q)
    matrix *= rotMat
}
func rotate(_ matrix: inout simd_float4x4,
    _ x: Float, _ y: Float, _ z: Float, _ w: Float
) { rotate(&matrix, simd_float4(x, y, z, w)); }
func rotate(_ matrix: inout simd_float4x4,
    _ euler: simd_float3
) { rotate(&matrix, toQuat(euler: euler)) }
func rotate(_ matrix: inout simd_float4x4,
    _ x: Float, _ y: Float, _ z:Float
) { rotate(&matrix, simd_float3(x, y, z)) }

func scale(_ matrix: inout simd_float4x4,
    _ xyz: simd_float3
) {
    matrix *= simd_float4x4(diagonal: simd_float4(xyz, 1.0))
}
func scale(_ matrix: inout simd_float4x4,
    _ x: Float, _ y: Float, _ z: Float
) { scale(&matrix, simd_float3(x, y, z)) }

func viewMatrix(_ eyePos: simd_float3, _ quaternion: simd_float4
) -> simd_float4x4 {
    // rotation matrix first,
    var mat = simd_float4x4(
        simd_quatf(vector: quaternion).inverse)
    translate(&mat, -eyePos)
    return mat
}
func viewMatrix(eyePos: simd_float3, euler: simd_float3
) -> simd_float4x4 {
    return viewMatrix(eyePos, toQuat(euler: euler)) }
func viewMatrix(eyePos: simd_float3, fwdDir: simd_float3,
    upDir: simd_float3
) -> simd_float4x4 {
    let xDir = normalize(simd_cross(upDir, fwdDir))
    let yDir = simd_cross(fwdDir, xDir)
    return matrix_from_rows(
        simd_float4(xDir, -dot(xDir, eyePos)),
        simd_float4(yDir, -dot(yDir, eyePos)),
        simd_float4(fwdDir, -dot(fwdDir, eyePos)),
        simd_float4(0, 0, 0, 1)
    )
}
func viewMatrix(eyePos: simd_float3, tgtPos: simd_float3,
    upDir: simd_float3
) -> simd_float4x4 {
    let fwdDir = normalize(tgtPos - eyePos)
    return viewMatrix(eyePos: eyePos, fwdDir: fwdDir,
        upDir: upDir)
}

func perspectiveMatrix(fov: Float, aspectRatio: Float,
    nearPlane: Float, farPlane: Float
) -> simd_float4x4 {
    let tanHalfFov = tan(fov / 2.0)

    var matrix = simd_float4x4(0.0)
    matrix[0][0] = 1.0 / (aspectRatio * tanHalfFov)
    matrix[1][1] = 1.0 / (tanHalfFov)
    matrix[2][2] = farPlane / (farPlane - nearPlane)
    matrix[2][3] = 1.0
    matrix[3][2] = -(farPlane * nearPlane) / (farPlane - nearPlane)

    return matrix
}

func upper3x3(_ mat: simd_float4x4) -> simd_float3x3 {
    return simd_float3x3(
        simd_float3(mat.columns.0.x, mat.columns.0.y, mat.columns.0.z),
        simd_float3(mat.columns.1.x, mat.columns.1.y, mat.columns.1.z),
        simd_float3(mat.columns.2.x, mat.columns.2.y, mat.columns.2.z)
    )
}

func normal(_ model: simd_float4x4
) -> simd_float3x3 {
    let upper = upper3x3(model)
    return simd_transpose(simd_inverse(upper))
}
