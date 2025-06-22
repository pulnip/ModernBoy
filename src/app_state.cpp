#include <vector>
#include "app_state.hpp"

using namespace ModernBoy;

template<>
std::vector<RawMesh> ModernBoy::import<RawMesh>(
    AppState& app, const std::string& filename
){ return app.meshImporter.import(filename); }

template<>
ResourceHandle<Mesh> ModernBoy::manage<Mesh>(
    AppState& app, Mesh&& resource
){ return app.meshManager.create(std::move(resource)); }
