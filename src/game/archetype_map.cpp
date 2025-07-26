#include <format>
#include <stdexcept>
#include <thread>
#include "log.hpp"
#include "util/thread_backoff.hpp"
#include "app_state.hpp"
#include "game/game_fwd.hpp"
#include "game/archetype_map.hpp"

#include <print>

using namespace ModernBoy;
using namespace ModernBoy::Game;

void RWPhaseGate::transform(Writer fn){
    on_write_phase();
    for(auto it=vec.begin(); it!=vec.end(); ++it)
        fn((*it).elmMem);
    write_phase_end();
}

template<>
void RWPhaseGate::mutate<void>(std::function<void(DynamicVector&)> fn){
    on_write_phase();
    fn(vec);
    write_phase_end();
}

size_t RWPhaseGate::size() const{
    size_t size;
    on_read_phase();
    size = vec.size();
    read_phase_end();
    return size;
}
void RWPhaseGate::free(Index i){
    on_write_phase();
    vec.remove(i);
    write_phase_end();
}

DynamicVector& RWPhaseGate::raw(){
    return vec;
}
const DynamicVector& RWPhaseGate::raw() const{
    return vec;
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

// Index ArchetypeMap::insert(ArchetypeBit bit,
//     const SparseChunk& chunk
// ){
//     size_t CHUNK_SIZE = size_of(bit);

//     if(archetypeMap.find(bit) == archetypeMap.end()){

//         auto [it, ret] = archetypeMap.try_emplace(bit, bit, CHUNK_SIZE);
//         GameDebug("    No Archetype: {} in map. emplace new. Result: {}", bit, ret);
//     }
//     auto& vector = archetypeMap.at(bit);
//     auto newIndex = vector.mutate<Index>([bit, &chunk](DynamicVector& vec){
//         auto newIndex = vec.insertRange(1);
//         auto mem = vec[newIndex];
//         setChunk(mem, chunk, bit);
//         return newIndex;
//     });
//     return newIndex;
// }
