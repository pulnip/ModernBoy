#include <algorithm>
#include <cassert>
#include <bit>
#include <limits>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

DynamicVector::DynamicVector(size_t CHUNK_SIZE)
:CHUNK_SIZE(CHUNK_SIZE){}

DynamicVector::DynamicVector(size_t CHUNK_SIZE, size_t initialSize)
:data(malloc(CHUNK_SIZE*initialSize)), CHUNK_SIZE(CHUNK_SIZE),
usedSize(initialSize), allocatedSize(std::bit_ceil(initialSize)),
maxUsedSize(initialSize)
{
    for(Index i=initialSize; i<allocatedSize; ++i){
        freeSlots.insert(i);
    }
}

DynamicVector::~DynamicVector(){
    if(data != nullptr)
        free(data);
}
DynamicVector::DynamicVector(DynamicVector&& other)
:CHUNK_SIZE(other.CHUNK_SIZE){
    moveFrom(std::move(other));
}
DynamicVector& DynamicVector::operator=(
    DynamicVector&& other
){
    moveFrom(std::move(other));
    return *this;
}
void DynamicVector::moveFrom(DynamicVector&& other){
    data = other.data;
    assert(CHUNK_SIZE == other.CHUNK_SIZE);
    usedSize= other.usedSize;
    allocatedSize = other.allocatedSize;
    maxUsedSize = other.maxUsedSize;
    freeSlots = other.freeSlots;
    other.data = nullptr;
}

Index DynamicVector::newChunk(size_t numChunk){
    assert(numChunk > 0);
    Index start = findContinuousFreeFittedSlot(numChunk);
    // not have free slot.
    if(start == std::numeric_limits<size_t>::max()){
        size_t newAllocSize = std::bit_ceil(maxUsedSize+numChunk);
        assert(allocatedSize < newAllocSize);
        if(usedSize == 0)
            data = malloc(CHUNK_SIZE*newAllocSize);
        else
            data = realloc(data, CHUNK_SIZE*newAllocSize);
        for(Index i=maxUsedSize; i<allocatedSize; ++i)
            freeSlots.erase(i);
        allocatedSize = newAllocSize;
        start = maxUsedSize;
        usedSize += numChunk;
        maxUsedSize += numChunk;
        for(Index i=start+numChunk; i<newAllocSize; ++i)
            freeSlots.insert(i);
        return start;
    }

    for(Index i=0; i<numChunk; ++i){
        freeSlots.erase(start+i);
    }
    usedSize += numChunk;
    maxUsedSize = std::max(maxUsedSize, start+numChunk);
    return start;
}

Index DynamicVector::findContinuousFreeFittedSlot(
    size_t numChunk
){
    assert((usedSize+freeSlots.size())==allocatedSize);
    if(allocatedSize-usedSize < numChunk)
        return std::numeric_limits<size_t>::max();

    Index candidateStart = *freeSlots.begin();;
    size_t numAvailable = 1;

    for(Index freeIndex: freeSlots){
        // Still Continuous
        if((freeIndex-candidateStart+1) == numAvailable){
            // find Continuous Chunk!
            if(numAvailable == numChunk){
                return candidateStart;
            }
            ++numAvailable;
        }
        else{
            candidateStart = freeIndex;
            numAvailable = 1;
        }
    }

    return std::numeric_limits<size_t>::max();
}

void DynamicVector::freeChunk(Index start, size_t numChunk){
    assert(numChunk <= usedSize);
    for(Index i=0; i<numChunk; ++i){
        freeSlots.insert(start+i);
    }
    usedSize -= numChunk;
    assert(start+numChunk<=maxUsedSize);
    if((start+numChunk) >= maxUsedSize){
        for(auto it=freeSlots.rbegin(); it!=freeSlots.rend(); ++it){
            if(maxUsedSize > *it + 1)
                break;
            else if(maxUsedSize - *it == 1)
                --maxUsedSize;
        }
    }
}

size_t DynamicVector::getChunkSize() const{
    return CHUNK_SIZE;
}

void* DynamicVector::operator[](Index index){
    [[likely]] if(index < allocatedSize)
        return Util::add(data, CHUNK_SIZE*index);
    return nullptr;
}
const void* DynamicVector::operator[](Index index) const{
    [[likely]] if(index < allocatedSize)
        return Util::add(data, CHUNK_SIZE*index);
    return nullptr;
}

size_t DynamicVector::size() const noexcept{
    assert(usedSize+freeSlots.size()==allocatedSize);
    return usedSize;
}
size_t DynamicVector::capacity() const noexcept{
    assert(usedSize+freeSlots.size()==allocatedSize);
    return allocatedSize;
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
    if(freeSlots.size()==0 || *freeSlots.begin()!=0)
        return Iterator(data, CHUNK_SIZE,
            0, maxUsedSize,
            freeSlots.cbegin(), freeSlots.cend());
    Index start = 0;
    auto it = freeSlots.cbegin();
    while(it!=freeSlots.cend() && start != *it){
        ++start;
        ++it;
    }
    return Iterator(data, CHUNK_SIZE,
        0, maxUsedSize,
        freeSlots.cbegin(), freeSlots.cend()
    );
}
Iterator DynamicVector::begin(Index i){
    return Iterator(data, CHUNK_SIZE,
        i, maxUsedSize,
        freeSlots.lower_bound(i), freeSlots.cend());
}
Iterator DynamicVector::end(){
    return Iterator(data, CHUNK_SIZE,
        maxUsedSize, maxUsedSize,
        freeSlots.find(maxUsedSize), freeSlots.cend());
}
ConstIt DynamicVector::begin() const{
    if(freeSlots.size()==0 || *freeSlots.begin()!=0)
        return ConstIt(data, CHUNK_SIZE,
            0, maxUsedSize,
            freeSlots.cbegin(), freeSlots.cend());
    Index start = 0;
    auto it = freeSlots.cbegin();
    while(it!=freeSlots.cend() && start != *it){
        ++start;
        ++it;
    }
    return ConstIt(data, CHUNK_SIZE,
        0, maxUsedSize,
        freeSlots.cbegin(), freeSlots.cend()
    );
}
ConstIt DynamicVector::begin(Index i) const{
    return ConstIt(data, CHUNK_SIZE,
        i, maxUsedSize,
        freeSlots.lower_bound(i), freeSlots.cend());
}
ConstIt DynamicVector::end() const{
    return ConstIt(data, CHUNK_SIZE,
        maxUsedSize, maxUsedSize,
        freeSlots.find(maxUsedSize), freeSlots.cend());
}
ConstIt DynamicVector::cbegin() const{
    if(freeSlots.size()==0 || *freeSlots.begin()!=0)
        return ConstIt(data, CHUNK_SIZE,
            0, maxUsedSize,
            freeSlots.cbegin(), freeSlots.cend());
    Index start = 0;
    auto it = freeSlots.cbegin();
    while(it!=freeSlots.cend() && start != *it){
        ++start;
        ++it;
    }
    return ConstIt(data, CHUNK_SIZE,
        0, maxUsedSize,
        freeSlots.cbegin(), freeSlots.cend()
    );
}
ConstIt DynamicVector::cbegin(Index i) const{
    return ConstIt(data, CHUNK_SIZE,
        i, maxUsedSize,
        freeSlots.lower_bound(i), freeSlots.cend());
}
ConstIt DynamicVector::cend() const{
    return ConstIt(data, CHUNK_SIZE,
        maxUsedSize, maxUsedSize,
        freeSlots.find(maxUsedSize), freeSlots.cend());
}

Iterator::Iterator(void* ptr, size_t STRIDE,
    Index index, Index maxUsedSize,
    std::set<size_t>::const_iterator it,
    std::set<size_t>::const_iterator it_end)
:ptr(ptr), STRIDE(STRIDE),
index(index), maxUsedSize(maxUsedSize),
it(it), it_end(it_end){}

void* Iterator::operator*(){
    return Util::add(ptr, STRIDE*index);
}
const void* Iterator::operator*() const{
    return Util::add(ptr, STRIDE*index);
}
Iterator& Iterator::operator++(){
    assert(index < maxUsedSize);
    ++index;
    if(it == it_end)
        return *this;
    if(index==maxUsedSize)
        return *this;
    if(index < *it)
        return *this;

    while(index != *it){
        ++index;
        ++it;
    }
    assert(index <= maxUsedSize);
    return *this;
}
bool Iterator::operator!=(const Iterator& other) const{
    return (ptr != other.ptr) || (index != other.index);
}
bool Iterator::operator==(const Iterator& other) const{
    return (ptr == other.ptr) && (index == other.index);
}

ConstIt::ConstIterator(const void* ptr, size_t STRIDE,
    Index index, Index maxUsedSize,
    std::set<size_t>::const_iterator it,
    std::set<size_t>::const_iterator it_end)
:ptr(ptr), STRIDE(STRIDE),
index(index), maxUsedSize(maxUsedSize),
it(it), it_end(it_end){}

const void* ConstIt::operator*() const{ return Util::add(ptr, STRIDE*index); }
ConstIt& ConstIt::operator++(){
    assert(index < maxUsedSize);
    ++index;
    if(it == it_end)
        return *this;
    if(index==maxUsedSize)
        return *this;
    if(index < *it)
        return *this;

    while(index != *it){
        ++index;
        ++it;
    }
    assert(index <= maxUsedSize);
    return *this;
}
bool ConstIt::operator!=(const ConstIt& other) const{
    return (ptr != other.ptr) || (index != other.index);
}
bool ConstIt::operator==(const ConstIt& other) const{
    return (ptr == other.ptr) && (index == other.index);
}

bool ModernBoy::operator==(
    const DynamicVector::ConstIterator& lhs,
    const DynamicVector::Iterator& rhs
){
    return (lhs.ptr == rhs.ptr) && (lhs.index == rhs.index);
}
bool ModernBoy::operator==(
    const DynamicVector::Iterator& lhs,
    const DynamicVector::ConstIterator& rhs
){
    return (lhs.ptr == rhs.ptr) && (lhs.index == rhs.index);
}