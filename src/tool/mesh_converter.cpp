#include <format>
#include <filesystem>
#include <print>
#include "engine/asset/mesh_importer.hpp"

namespace fs = std::filesystem;
using namespace ModernBoy::Asset;

auto main(int argc, char* argv[])->int{
    if(argc < 5){
        std::println("usage: [--in] <input path> [--out] <output dir>");
        return 1;
    }

    fs::path input_path;
    fs::path output_dir;

    for(int i=1; i<argc; ++i){
        std::string arg = argv[i];
        if((arg == "--in") && i+1 < argc)
            input_path = argv[++i];
        else if((arg == "--out") && i+1 < argc)
            output_dir = argv[++i];
    }
    if(input_path.empty()){
        std::println("input file not specified");
        return 1;
    }
    if(output_dir.empty()){
        std::println("output directory not specified");
        return 1;
    }

    if(!fs::exists(input_path) || !fs::is_regular_file(input_path)){
        std::println("no such file: {}", input_path.string());
        return 1;
    }
    if(!fs::exists(output_dir) || !fs::is_directory(output_dir)){
        std::println("no such directory: {}", output_dir.string());
        return 1;
    }

    auto ext = input_path.extension().string();
    fs::path output_path = output_dir / (input_path.stem().string() + ".mbmesh");

    for(char& c : ext) c = (char)std::tolower((unsigned char)c);
    if(ext == ".fbx" || ext == ".gltf" || ext == ".glb"){
        auto imported = importMeshFile(input_path);
        serialize(imported, output_path);
        return 0;
    } else {
        std::println("{} not supported: {}", ext, input_path.string());
        return 1;
    }
}