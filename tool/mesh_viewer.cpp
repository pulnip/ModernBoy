#include <filesystem>
#include <print>
#include "asset/mesh_importer.hpp"

namespace fs = std::filesystem;
using namespace ModernBoy::Asset;

int main(int argc, char* argv[]){
    if(argc < 3){
        std::println("usage: {} --target <file.mbmesh>", argv[0]);
        return 1;
    }
    fs::path target;
    for(int i=1;i<argc;++i){
        std::string arg = argv[i];
        if((arg=="--target" || arg=="-t") && i+1<argc){ target = argv[++i]; }
    }
    if(target.empty()){
        std::println("[ERR] --target <file> is required");
        return 1;
    }
    if(!fs::exists(target) || !fs::is_regular_file(target)){
        std::println("[ERR] no such file: {}", target.string());
        return 1;
    }

    auto ext = target.extension().string();
    if(ext != ".mbmesh"){
        std::println("[ERR] target extension is not correct: {}", target.string());
        return 1;
    }

    auto imported = loadMeshFile(target);
    printLoadedMesh(imported);

    return 0;
}
