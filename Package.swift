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
            path: "src/engine/render/metal",
            exclude: [
                "mesh.cpp",
                "shader.cpp",
                "texture.cpp",
            ]
        )
    ],
)
