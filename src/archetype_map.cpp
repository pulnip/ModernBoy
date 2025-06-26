#include "archetype_map.hpp"
#include "util/bit.hpp"
#include "app_state.hpp"
#include "util/thread_backoff.hpp"

using namespace ModernBoy;

static size_t bit_size(ArchetypeBit bit);
static void setChunk(void* dst, const SparseChunk& chunk,
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
    fn(vec.raw());
    read_phase_end();
}
void RWPhaseGate::custom_write(Writer fn){
    on_write_phase();
    fn(vec.raw());
    write_phase_end();
}

size_t RWPhaseGate::size() const{
    on_read_phase();
    return vec.size();
    read_phase_end();
}
void RWPhaseGate::get(Index i, void* dst) const{
    on_read_phase();
    memcpy(dst, vec[i], vec.getChunkSize());
    read_phase_end();
}
void RWPhaseGate::free(Index i){
    on_write_phase();
    vec.freeChunk(i);
    write_phase_end();
}



void RWPhaseGate::on_read_phase() const{
    AdaptiveBackoff backoff = {};

    while(true){
        uint_fast32_t s = state.load(std::memory_order_acquire);
        if(s & WRITER_BIT)
            backoff();
        if(state.compare_exchange_weak(s, s+1,
            std::memory_order_acquire)) break;
    }
}
void RWPhaseGate::read_phase_end() const{
    state.fetch_sub(1, std::memory_order_release);
}
void RWPhaseGate::on_write_phase(){
    AdaptiveBackoff backoff{};
    while(true){
        uint_fast32_t expected = state.load(std::memory_order_acquire);
        if(expected & WRITER_BIT)
            continue;
        if(state.compare_exchange_weak(
            expected, expected | WRITER_BIT,
            std::memory_order_acq_rel
        )) break;
    }
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
        archetypeMap.try_emplace(bit, CHUNK_SIZE, 8);
    auto& vector = archetypeMap.at(bit);
    auto newIndex = vector.mutate<Index>([bit, &chunk](DynamicVector& vec){
        auto newIndex = vec.newChunk(1);
        auto mem = vec[newIndex];
        setChunk(mem, chunk, bit);
        return newIndex;
    });
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
