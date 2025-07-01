#include <format>
#include <stdexcept>
#include <thread>
#include "archetype_map.hpp"
#include "util/bit.hpp"
#include "app_state.hpp"
#include "util/thread_backoff.hpp"

#include <print>

using namespace ModernBoy;

static size_t bit_size(ArchetypeBit bit);
static void setChunk(void* dst, const SparseChunk& chunk,
    ArchetypeBit bit);
static void setChunk(void* dst, const TransformComponent& component,
    ArchetypeBit bit);
static void setChunk(void* dst, const CameraComponent& component,
    ArchetypeBit bit);
static void setChunk(void* dst, const MeshComponent& component,
    ArchetypeBit bit);
static void setChunk(void* dst, const InputComponent& component,
    ArchetypeBit bit);

void RWPhaseGate::for_each(Reader fn) const{
    on_read_phase();
    for(auto it=vec.cbegin(); it!=vec.cend(); ++it)
        fn(*it);
    read_phase_end();
}
void RWPhaseGate::transform(Writer fn){
    on_write_phase();
    for(auto it=vec.begin(); it!=vec.end(); ++it)
        fn(*it);
    write_phase_end();
}
void RWPhaseGate::transform_range(
    Writer fn, Index start, size_t end
){
    on_write_phase();
    auto end_it = vec.begin(end);
    for(auto it = vec.begin(start); it!=end_it; ++it){
        fn(*it);
    }
    write_phase_end();
}

template<>
void RWPhaseGate::mutate<void>(std::function<void(DynamicVector&)> fn){
    on_write_phase();
    fn(vec);
    write_phase_end();
}

void RWPhaseGate::custom_read(Reader fn) const{
    on_read_phase();
    fn(vec.data());
    read_phase_end();
}
void RWPhaseGate::custom_write(Writer fn){
    on_write_phase();
    fn(vec.data());
    write_phase_end();
}

size_t RWPhaseGate::size() const{
    size_t size;
    on_read_phase();
    size = vec.size();
    read_phase_end();
    return size;
}
void RWPhaseGate::get(Index i, void* dst) const{
    on_read_phase();
    memcpy(dst, vec[i], vec.elmSize());
    read_phase_end();
}
void RWPhaseGate::free(Index i){
    on_write_phase();
    vec.remove(i);
    write_phase_end();
}



void* RWPhaseGate::operator[](Index index){
    return vec[index];
}
const void* RWPhaseGate::operator[](Index index) const{
    return vec[index];
}

thread_local int read_phase_depth = 0;

void RWPhaseGate::on_read_phase() const{
    AdaptiveBackoff backoff = {};

    while(true){
        uint_fast32_t s = state.load(std::memory_order_acquire);
        if(s & WRITER_BIT)
            continue;
        else if(state.compare_exchange_weak(s, s+1,
            std::memory_order_acquire
        )) break;
        backoff();
    }
    ++read_phase_depth;
}
void RWPhaseGate::read_phase_end() const{
    assert((state.load() & READER_MASK) != 0);
    state.fetch_sub(1, std::memory_order_release);
    --read_phase_depth;
}
void RWPhaseGate::on_write_phase(){
    // greedy thread
    assert(read_phase_depth == 0);

    AdaptiveBackoff backoff{};
    while(true){
        uint_fast32_t expected = state.load(std::memory_order_acquire);
        if(expected & WRITER_BIT)
            continue;
        else if(state.compare_exchange_weak(
            expected, expected | WRITER_BIT,
            std::memory_order_acq_rel
        )) break;
        backoff();
    }
    backoff.reset();
    while((state.load(std::memory_order_acquire)
        & READER_MASK) != 0
    ) backoff();
}
void RWPhaseGate::write_phase_end(){
    state.fetch_and(~WRITER_BIT, std::memory_order_release);
}

RWPhaseGate& ArchetypeMap::at(ArchetypeBit bit){
    return archetypeMap.at(bit);
}
const RWPhaseGate& ArchetypeMap::at(ArchetypeBit bit) const{
    return archetypeMap.at(bit);
}

Index ArchetypeMap::insert(ArchetypeBit bit,
    const SparseChunk& chunk
){
    size_t CHUNK_SIZE = bit_size(bit);

    if(archetypeMap.find(bit) == archetypeMap.end())
        archetypeMap.try_emplace(bit, CHUNK_SIZE);
    auto& vector = archetypeMap.at(bit);
    auto newIndex = vector.mutate<Index>([bit, &chunk](DynamicVector& vec){
        auto newIndex = vec.insertRange(1);
        auto mem = vec[newIndex];
        setChunk(mem, chunk, bit);
        return newIndex;
    });
    return newIndex;
}

TransformComponent ArchetypeMap::getTransformComponent(
    ArchetypeBit bit, Index index
){
    auto tc = dangled<TransformComponent>();
    auto cc = dangled<CameraComponent>();
    auto mc = dangled<MeshComponent>();
    auto ic = dangled<InputComponent>();
    if(bit & TRANSFORM_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_read_phase();
        getChunk(&tc, &cc, &mc, &ic, vec[index], bit);
        vec.read_phase_end();
    }
    return tc;
}
CameraComponent ArchetypeMap::getCameraComponent(
    ArchetypeBit bit, Index index
){
    auto tc = dangled<TransformComponent>();
    auto cc = dangled<CameraComponent>();
    auto mc = dangled<MeshComponent>();
    auto ic = dangled<InputComponent>();
    if(bit & CAMERA_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_read_phase();
        getChunk(&tc, &cc, &mc, &ic, vec[index], bit);
        vec.read_phase_end();
    }
    return cc;
}
MeshComponent ArchetypeMap::getMeshComponent(
    ArchetypeBit bit, Index index
){
    auto tc = dangled<TransformComponent>();
    auto cc = dangled<CameraComponent>();
    auto mc = dangled<MeshComponent>();
    auto ic = dangled<InputComponent>();
    if(bit & MESH_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_read_phase();
        getChunk(&tc, &cc, &mc, &ic, vec[index], bit);
        vec.read_phase_end();
    }
    return mc;
}
InputComponent ArchetypeMap::getInputComponent(
    ArchetypeBit bit, Index index
){
    auto tc = dangled<TransformComponent>();
    auto cc = dangled<CameraComponent>();
    auto mc = dangled<MeshComponent>();
    auto ic = dangled<InputComponent>();
    if(bit & INPUT_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_read_phase();
        getChunk(&tc, &cc, &mc, &ic, vec[index], bit);
        vec.read_phase_end();
    }
    return ic;
}
void ArchetypeMap::setTransformComponent(
    const TransformComponent& component,
    ArchetypeBit bit, Index index
){
    if(bit & TRANSFORM_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_write_phase();
        setChunk(vec[index], component, bit);
        vec.write_phase_end();
    }
}
void ArchetypeMap::setCameraComponent(
    const CameraComponent& component,
    ArchetypeBit bit, Index index
){
    if(bit & CAMERA_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_write_phase();
        setChunk(vec[index], component, bit);
        vec.write_phase_end();
    }
}
void ArchetypeMap::setMeshComponent(
    const MeshComponent& component,
    ArchetypeBit bit, Index index
){
    if(bit & MESH_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_write_phase();
        setChunk(vec[index], component, bit);
        vec.write_phase_end();
    }
}
void ArchetypeMap::setInputComponent(
    const InputComponent& component,
    ArchetypeBit bit, Index index
){
    if(bit & INPUT_BIT){
        auto& vec=archetypeMap.at(bit);
        vec.on_write_phase();
        setChunk(vec[index], component, bit);
        vec.write_phase_end();
    }
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

static void setChunk(void* dst, const TransformComponent& component,
    ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::chunkcpy(dst, component);
}
static void setChunk(void* dst, const CameraComponent& component,
    ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::add<TransformComponent>(dst);
    if(bit & CAMERA_BIT)
        dst = Util::chunkcpy(dst, component);
}
static void setChunk(void* dst, const MeshComponent& component,
    ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::add<TransformComponent>(dst);
    if(bit & CAMERA_BIT)
        dst = Util::add<CameraComponent>(dst);
    if(bit & MESH_BIT)
        dst = Util::chunkcpy(dst, component);

}
static void setChunk(void* dst, const InputComponent& component,
    ArchetypeBit bit
){
    if(bit & TRANSFORM_BIT)
        dst = Util::add<TransformComponent>(dst);
    if(bit & CAMERA_BIT)
        dst = Util::add<CameraComponent>(dst);
    if(bit & MESH_BIT)
        dst = Util::add<InputComponent>(dst);
    if(bit & INPUT_BIT)
        dst = Util::chunkcpy(dst, component);
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
    if(bit & INPUT_BIT)
        dst = Util::chunkcpy(dst, chunk.input);
}

void ModernBoy::getChunk(TransformComponent* tc,
    CameraComponent* cc, MeshComponent* mc,
    InputComponent* ic, const void* src,
    ArchetypeBit bit
){
    TransformComponent tc_tmp;
    CameraComponent cc_tmp;
    MeshComponent mc_tmp;
    InputComponent ic_tmp;

    if(bit & TRANSFORM_BIT)
        src = Util::chunkcpy(tc_tmp, src);
    if(bit & CAMERA_BIT)
        src = Util::chunkcpy(cc_tmp, src);
    if(bit & MESH_BIT)
        src = Util::chunkcpy(mc_tmp, src);
    if(bit & INPUT_BIT)
        src = Util::chunkcpy(ic_tmp, src);

    if(tc != nullptr)
        *tc = tc_tmp;
    if(cc != nullptr)
        *cc = cc_tmp;
    if(mc != nullptr)
        *mc = mc_tmp;
    if(ic != nullptr)
        *ic = ic_tmp;
}
