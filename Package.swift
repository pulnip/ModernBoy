// swift-tools-version: 6.1
// The swift-tools-version declares the minimum version of Swift required to build this package.

import PackageDescription

let package = Package(
    name: "ModernBoy",
    platforms: [
        .macOS(.v15)
    ],
    targets: [
        .target(
            name: "ModernBoy",
            path: "engine/src/render/metal",
            exclude: [
                "material.cpp",
                "mesh.cpp",
                "shader.cpp",
                "CMakeLists.txt",
            ],
            swiftSettings: [
                .unsafeFlags(
                    [
                        "-import-objc-header",
                        "engine/include/render/ktx2_transcode.h",
                    ]
                )
            ]
        )
    ]
)
