#include <algorithm>
#include <cassert>
#include <bit>
#include <limits>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

DynamicVector::DynamicVector(size_t CHUNK_SIZE)
:CHUNK_SIZE(CHUNK_SIZE){}

DynamicVector::DynamicVector(size_t CHUNK_SIZE, size_t initialSize)
:CHUNK_SIZE(CHUNK_SIZE), maxSize(std::bit_ceil(initialSize)),
data(malloc(CHUNK_SIZE*maxSize))
{
    for(Index i=0; i<=maxSize; ++i){
        freeSlots.insert(i);
    }
}

DynamicVector::~DynamicVector(){
    if(data != nullptr)
        free(data);
}
DynamicVector::DynamicVector(DynamicVector&& other){
    moveFrom(std::move(other)); }
DynamicVector& DynamicVector::operator=(
    DynamicVector&& other
){
    moveFrom(std::move(other));
    return *this;
}
void DynamicVector::moveFrom(DynamicVector&& other){
    assert(CHUNK_SIZE == other.CHUNK_SIZE);
    maxSize = other.maxSize;
    data = other.data;
    size_ = other.size_;
    freeOnlyIndex = other.freeOnlyIndex;
    freeSlots = other.freeSlots;
#ifdef _DEBUG
    numChunk_last = other.numChunk_last;
#endif
    other.data = nullptr;
#ifdef _DEBUG
    other.numChunk_last = 0;
#endif
}

Index DynamicVector::newChunk(size_t numChunk){
    assert(numChunk > 0);
#ifdef _DEBUG
    if(numChunk_last % numChunk != 0)
        perror("[Warning] Memory Fragment Warning");
    numChunk_last = numChunk;
#endif
    Index start = findContinuousFreeFittedSlot(numChunk);
    // not have free slot.
    if(start == std::numeric_limits<size_t>::max()){
        size_t newMaxSize = std::bit_ceil(freeOnlyIndex+numChunk);
        assert(maxSize < newMaxSize);
        if(size_ == 0)
            data = malloc(newMaxSize);
        else
            data = realloc(data, newMaxSize);
        for(Index i=maxSize+1; i<=newMaxSize; ++i){
            freeSlots.insert(i);
        }
        maxSize = newMaxSize;
        start = findContinuousFreeFittedSlot(numChunk);
    }

    for(Index i=0; i<numChunk; ++i){
        freeSlots.erase(start+i);
    }
    size_ += numChunk;
    freeOnlyIndex = std::max(start+numChunk, freeOnlyIndex);
    return start;
}

Index DynamicVector::findContinuousFreeFittedSlot(
    size_t numChunk
){
    assert((size_+freeSlots.size())==(maxSize+1));
    if(maxSize-size_ < numChunk)
        return std::numeric_limits<size_t>::max();

    Index candidateIndex = *freeSlots.cbegin();
    size_t numAvailable = 0;

    for(Index freeIndex: freeSlots){
        numAvailable += 1;
        // Still Continuous
        if((freeIndex-candidateIndex) == numAvailable){
            // find Continuous Chunk!
            if(numAvailable == numChunk){
                return candidateIndex;
            }
        }
        else if(freeIndex == maxSize){
            break;
        }
        else{
            candidateIndex = freeIndex;
            numAvailable = 0;
        }
    }

    return std::numeric_limits<size_t>::max();
}

void DynamicVector::freeChunk(Index start, size_t numChunk){
    assert(numChunk <= size_);
    for(Index i=0; i<numChunk; ++i)
        freeSlots.insert(start+i);
    size_ -= numChunk;
    assert(start+numChunk<=freeOnlyIndex);
    if(start+numChunk==freeOnlyIndex)
        freeOnlyIndex = start;
}

size_t DynamicVector::getChunkSize() const{
    return CHUNK_SIZE;
}

void* DynamicVector::operator[](Index index){
    [[likely]] if(index < maxSize)
        return static_cast<uint8_t*>(data) + CHUNK_SIZE*index;
    return nullptr;
}
const void* DynamicVector::operator[](Index index) const{
    [[likely]] if(index < maxSize)
        return static_cast<uint8_t*>(data) + CHUNK_SIZE*index;
    return nullptr;
}

size_t DynamicVector::size() const noexcept{
    assert(size_+freeSlots.size()==(maxSize+1));
    return size_;
}
size_t DynamicVector::capacity() const noexcept{
    assert(size_+freeSlots.size()==(maxSize+1));
    return maxSize;
}

void* DynamicVector::raw() noexcept{
    return data;
}
const void* DynamicVector::raw() const noexcept{
    return data;
}

using Iterator = DynamicVector::Iterator;
using ConstIt = DynamicVector::ConstIterator;

Iterator DynamicVector::begin(){
    return Iterator(data, 0, CHUNK_SIZE,
        freeSlots.cbegin());
}
Iterator DynamicVector::begin(Index i){
    return Iterator(data, i, CHUNK_SIZE,
        freeSlots.lower_bound(i));
}
Iterator DynamicVector::end(){
    assert(freeSlots.find(freeOnlyIndex)
        != freeSlots.end());
    return Iterator(data, size_, CHUNK_SIZE,
        freeSlots.find(freeOnlyIndex));
}
ConstIt DynamicVector::begin() const{
    return ConstIt(data, 0, CHUNK_SIZE,
        freeSlots.cbegin());
}
ConstIt DynamicVector::begin(Index i) const{
    return ConstIt(data, i, CHUNK_SIZE,
        freeSlots.lower_bound(i));
}
ConstIt DynamicVector::end() const{
    assert(freeSlots.find(freeOnlyIndex)
        != freeSlots.end());
    return ConstIt(data, 0, CHUNK_SIZE,
        freeSlots.find(freeOnlyIndex));
}
ConstIt DynamicVector::cbegin() const{
    return ConstIt(data, 0, CHUNK_SIZE,
        freeSlots.cbegin());
}
ConstIt DynamicVector::cbegin(Index i) const{
    return ConstIt(data, 0, CHUNK_SIZE,
        freeSlots.lower_bound(i));
}
ConstIt DynamicVector::cend() const{
    assert(freeSlots.find(freeOnlyIndex)
        != freeSlots.end());
    return ConstIt(data, size_, CHUNK_SIZE,
        freeSlots.find(freeOnlyIndex));
}

Iterator::Iterator(void* ptr, Index index, size_t STRIDE,
    std::set<size_t>::const_iterator it)
:ptr(ptr),index(index),STRIDE(STRIDE),it(it){}

void* Iterator::operator*(){
    return Util::add(ptr, STRIDE*index);
}
const void* Iterator::operator*() const{
    return Util::add(ptr, STRIDE*index);
}
Iterator& Iterator::operator++(){
    do{
        index += 1;
    } while(*it < index);
    ++it;
    return *this;
}
bool Iterator::operator!=(const Iterator& other) const{
    return (ptr != other.ptr) || (index != other.index);
}
bool Iterator::operator==(const Iterator& other) const{
    return (ptr == other.ptr) && (index == other.index);
}

ConstIt::ConstIterator(const void* ptr, Index index, size_t STRIDE,
    std::set<size_t>::const_iterator it)
:ptr(ptr),index(index),STRIDE(STRIDE),it(it){}

const void* ConstIt::operator*() const{ return Util::add(ptr, STRIDE*index); }
ConstIt& ConstIt::operator++(){
    do{
        index += 1;
    } while(*it < index);
    ++it;
    return *this;
}
bool ConstIt::operator!=(const ConstIt& other) const{
    return (ptr != other.ptr) || (index != other.index);
}
bool ConstIt::operator==(const ConstIt& other) const{
    return (ptr == other.ptr) && (index == other.index);
}
