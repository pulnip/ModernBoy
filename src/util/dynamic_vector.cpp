#include <cassert>
#include <bit>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

DynamicVector::DynamicVector(size_t CHUNK_SIZE)
:CHUNK_SIZE(CHUNK_SIZE){}

DynamicVector::DynamicVector(size_t CHUNK_SIZE, size_t initialSize)
:CHUNK_SIZE(CHUNK_SIZE), maxSize(std::bit_ceil(initialSize)), data(malloc(CHUNK_SIZE*maxSize)),
size(initialSize){
    for(Index i=initialSize; i<maxSize; ++i){
        freeSlots.insert(i);
    }
}

DynamicVector::~DynamicVector(){
    free(data);
}

Index DynamicVector::newChunk(size_t numChunk){
    assert(numChunk > 0);
#ifdef _DEBUG || DEBUG
    if(numChunk_last % numChunk != 0)
        perror("[Warning] Memory Fragment Warning");
    numChunk_last = numChunk;
#endif
    Index start = findContinuousFreeFittedSlot(numChunk);
    // already have free slot.
    if(start != size_t(-1)){
        return start;
    }

    size_t newMaxSize = std::bit_ceil(size+numChunk);
    realloc(data, newMaxSize);
    for(Index i=maxSize; i<newMaxSize; ++i){
        freeSlots.insert(i);
    }
    maxSize = newMaxSize;
    return findContinuousFreeFittedSlot(numChunk);
}

Index DynamicVector::findContinuousFreeFittedSlot(
    size_t numChunk
){
    assert(freeSlots.size()==(maxSize-size));
    if(maxSize-size < numChunk)
        return size_t(-1);

    Index candidateIndex = *freeSlots.cbegin();
    size_t numAvailable = 0;

    for(Index freeIndex: freeSlots){
        numAvailable += 1;
        // Still Continuous
        if((freeIndex-candidateIndex) == numAvailable){
            // find Continuous Chunk!
            if(numAvailable == numChunk)
                return candidateIndex;
        }
        else{
            candidateIndex = freeIndex;
            numAvailable = 0;
        }
    }

    return size_t(-1);
}

void DynamicVector::freeChunk(Index startIndex, size_t numChunk){
    for(Index i=startIndex; i<numChunk; ++i)
        freeSlots.insert(i);
}

void* DynamicVector::operator[](Index index){
    [[likely]] if(index < maxSize)
        return static_cast<uint8_t*>(data) + CHUNK_SIZE*index;
    return nullptr;
}

void* ModernBoy::getChunkData(void* chunk, size_t byteOffset){
    return static_cast<uint8_t*>(chunk)+byteOffset;
}
void ModernBoy::setChunkData(const void* data, void* chunk, size_t byteOffset){
    memcpy(static_cast<uint8_t*>(chunk)+byteOffset,
        data, byteOffset
    );
}

