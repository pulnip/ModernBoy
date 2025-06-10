import Foundation
import simd

func toRadians(from degree: Float) -> Float {
    return degree * .pi / 180.0;
}

func rotate(_ matrix: inout simd_float4x4,
    _ x: Float, _ y: Float, _ z:Float
) {
    let rotation = simd_float3(x, y, z)
    //Create quaternion
    let c = cos(rotation * 0.5);
    let s = sin(rotation * 0.5);

    var quat = simd_float4(repeating: 1.0);

    quat.w = c.x * c.y * c.z + s.x * s.y * s.z;
    quat.x = s.x * c.y * c.z - c.x * s.y * s.z;
    quat.y = c.x * s.y * c.z + s.x * c.y * s.z;
    quat.z = c.x * c.y * s.z - s.x * s.y * c.z;

    //Create matrix
    var rotationMat = matrix_identity_float4x4;
    let qxx = quat.x * quat.x;
    let qyy = quat.y * quat.y;
    let qzz = quat.z * quat.z;
    let qxz = quat.x * quat.z;
    let qxy = quat.x * quat.y;
    let qyz = quat.y * quat.z;
    let qwx = quat.w * quat.x;
    let qwy = quat.w * quat.y;
    let qwz = quat.w * quat.z;

    rotationMat[0][0] = 1.0 - 2.0 * (qyy + qzz);
    rotationMat[0][1] = 2.0 * (qxy + qwz);
    rotationMat[0][2] = 2.0 * (qxz - qwy);

    rotationMat[1][0] = 2.0 * (qxy - qwz);
    rotationMat[1][1] = 1.0 - 2.0 * (qxx + qzz);
    rotationMat[1][2] = 2.0 * (qyz + qwx);

    rotationMat[2][0] = 2.0 * (qxz + qwy);
    rotationMat[2][1] = 2.0 * (qyz - qwx);
    rotationMat[2][2] = 1.0 - 2.0 * (qxx + qyy);

    matrix *= rotationMat;
}

func viewMatrix(eyePos: simd_float3, tgtPos: simd_float3,
    upDir: simd_float3
) -> simd_float4x4 {
    let fwdDir = normalize(tgtPos - eyePos)
    let rightDir = normalize(simd_cross(upDir, fwdDir))
    let upDir = simd_cross(fwdDir, rightDir)

    var matrix = matrix_identity_float4x4;
    matrix[0][0] = rightDir.x;
    matrix[1][0] = rightDir.y;
    matrix[2][0] = rightDir.z;
    matrix[0][1] = upDir.x;
    matrix[1][1] = upDir.y;
    matrix[2][1] = upDir.z;
    matrix[0][2] = fwdDir.x;
    matrix[1][2] = fwdDir.y;
    matrix[2][2] = fwdDir.z;
    matrix[3][0] = -dot(rightDir, eyePos);
    matrix[3][1] = -dot(upDir, eyePos);
    matrix[3][2] = -dot(fwdDir, eyePos);

    return matrix;
}

func perspectiveMatrix(fov: Float, aspectRatio: Float,
    nearPlane: Float, farPlane: Float
) -> simd_float4x4 {
    let tanHalfFov = tan(fov / 2.0);

    var matrix = simd_float4x4(0.0);
    matrix[0][0] = 1.0 / (aspectRatio * tanHalfFov);
    matrix[1][1] = 1.0 / (tanHalfFov);
    matrix[2][2] = farPlane / (farPlane - nearPlane);
    matrix[2][3] = 1.0;
    matrix[3][2] = -(farPlane * nearPlane) / (farPlane - nearPlane);

    return matrix;
}
