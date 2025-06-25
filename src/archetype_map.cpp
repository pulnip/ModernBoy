#include "archetype_map.hpp"
#include "util/bit.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

static size_t bit_size(ArchetypeBit bit);
static void setChunk(void* dst, const SparseChunk& chunk,
    ArchetypeBit bit);

DynamicVector& ArchetypeMap::at(ArchetypeBit bit){
    return archetypeMap.at(bit);
}
const DynamicVector& ArchetypeMap::at(ArchetypeBit bit) const{
    return archetypeMap.at(bit);
}

Index ArchetypeMap::insert(ArchetypeBit bit,
    const SparseChunk& chunk
){
    size_t CHUNK_SIZE = bit_size(bit);

    if(archetypeMap.find(bit) == archetypeMap.end())
        archetypeMap.emplace(bit, DynamicVector(CHUNK_SIZE, 8));
    auto& vector = archetypeMap.at(bit);
    auto newIndex = vector.newChunk(1);
    auto mem = vector[newIndex];

    setChunk(mem, chunk, bit);
    return newIndex;
}

static size_t bit_size(ArchetypeBit bit){
    size_t size = 0;
    if(bit & TRANSFORM_BIT)
        size += sizeof(TransformComponent);
    if(bit & CAMERA_BIT)
        size += sizeof(CameraComponent);
    if(bit & MESH_BIT)
        size += sizeof(MeshComponent);
    if(bit & INPUT_BIT)
        size += sizeof(InputComponent);
    return size;
}

static void setChunk(void* dst, const SparseChunk& chunk,
    ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::chunkcpy(dst, chunk.transform);
    if(bit & CAMERA_BIT)
        dst = Util::chunkcpy(dst, chunk.camera);
    if(bit & MESH_BIT)
        dst = Util::chunkcpy(dst, chunk.mesh);
    // if(bit & INPUT_BIT)
    //     dst = Util::chunkcpy(dst, chunk.input);
}

void ModernBoy::getChunk(TransformComponent* tc,
    CameraComponent* cc, MeshComponent* mc,
    InputComponent* ic, const void* src
){
    if(tc != nullptr)
        src = Util::chunkcpy(*tc, src);
    if(cc != nullptr)
        src = Util::chunkcpy(*cc, src);
    if(mc != nullptr)
        src = Util::chunkcpy(*mc, src);
    // if(ic != nullptr)
    //     src = Util::chunkcpy(*ic, src);
}
