#include "archetype_map.hpp"
#include "app_state.hpp"

using namespace ModernBoy;

static size_t bit_size(ArchetypeBit bit);
static void setChunk(void* dst, const SparseChunk& chunk,
    ArchetypeBit bit);

void ArchetypeMap::add(EntityID actor, ArchetypeBit bit,
    const SparseChunk& chunk
){
    size_t CHUNK_SIZE = bit_size(bit);

    if(archetypeMap.find(bit) == archetypeMap.end())
        archetypeMap.emplace(bit, DynamicVector(CHUNK_SIZE, 8));
    auto& vector = archetypeMap.at(bit);
    auto newIndex = vector.newChunk(1);
    auto mem = vector[newIndex];

    setChunk(mem, chunk, bit);
    auto archetypeInfo = std::make_pair(bit, newIndex);
    actor_info.emplace(std::make_pair(actor, archetypeInfo));
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
    size_t offset = 0;

    if(bit & TRANSFORM_BIT){
        setChunkData(&(chunk.transform), dst, offset);
        offset += sizeof(TransformComponent);
    }
    if(bit & CAMERA_BIT){
        setChunkData(&(chunk.camera), dst, offset);
        offset += sizeof(CameraComponent);
    }
    if(bit & MESH_BIT){
        setChunkData(&(chunk.mesh), dst, offset);
        offset += sizeof(MeshComponent);
    }
    if(bit & INPUT_BIT){
        setChunkData(&(chunk.input), dst, offset);
        offset += sizeof(InputComponent);
    }
}
