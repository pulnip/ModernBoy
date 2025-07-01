#include <algorithm>
#include <cassert>
#include <bit>
#include <iostream>
#include <limits>
#include "util/dynamic_vector.hpp"

using namespace ModernBoy;

DynamicVectorBadCast::DynamicVectorBadCast(
    size_t expected, size_t requested
):msg(std::format("bad cast: expected={}bytes, requested={}bytes", expected, requested)){}
const char* DynamicVectorBadCast::what(
) const noexcept{ return msg.c_str(); }

DynamicVector::DynamicVector(size_t ELEMENT_SIZE) noexcept
:ELEMENT_SIZE(ELEMENT_SIZE){}

DynamicVector::DynamicVector(size_t ELEMENT_SIZE, size_t initialSize) noexcept
:mem(malloc(ELEMENT_SIZE*initialSize)), ELEMENT_SIZE(ELEMENT_SIZE),
numElement(initialSize), memSize(std::bit_ceil(initialSize)),
usedSize(initialSize){
    for(Index i=initialSize; i<memSize; ++i){
        freeIndexes.insert(i);
    }
}

size_t DynamicVector::checkSize(size_t elmSize) const{
    if(ELEMENT_SIZE != elmSize)
        throw DynamicVectorBadCast(ELEMENT_SIZE, elmSize);
    return elmSize;
}

DynamicVector::~DynamicVector(){
    if(mem != nullptr)
        free(mem);
}
DynamicVector::DynamicVector(DynamicVector&& other)
:ELEMENT_SIZE(checkSize(other.ELEMENT_SIZE)){
    moveFrom(std::move(other));
}
DynamicVector& DynamicVector::operator=(
    DynamicVector&& other
){
    moveFrom(std::move(other));
    return *this;
}
void DynamicVector::moveFrom(DynamicVector&& other){
    mem = other.mem;
    numElement= other.numElement;
    memSize = other.memSize;
    usedSize = other.usedSize;
    freeIndexes = other.freeIndexes;
    other.mem = nullptr;
}

void* DynamicVector::at(Index index){
    if(index >= memSize)
        throw std::out_of_range(std::format(
            "Index {} is out of valid range (0~{}).",
            index, memSize));
    if(freeIndexes.find(index) != freeIndexes.end())
        throw std::out_of_range(std::format(
            "Index {} is not inserted before", index));
    return (*this)[index];
}
const void* DynamicVector::at(Index index, size_t offset) const{
    if(index >= memSize)
        throw std::out_of_range(std::format(
            "Index {} is not inserted before",
            index));
    if(offset > ELEMENT_SIZE)
        throw std::out_of_range(std::format(
            "Offset {} is out of valid range (0~{})",
            offset, ELEMENT_SIZE));
    return Util::add(mem, ELEMENT_SIZE*index+offset);
}
const void* DynamicVector::at(Index index) const{
    if(index >= memSize)
        throw std::out_of_range(std::format(
            "Index {} is out of valid range (0~{}).",
            index, memSize));
    if(freeIndexes.find(index) != freeIndexes.end())
        throw std::out_of_range(std::format(
            "Index {} is not inserted before", index));
    return (*this)[index];
}
void* DynamicVector::at(Index index, size_t offset){
    if(index >= memSize)
        throw std::out_of_range(std::format(
            "Index {} is not inserted before",
            index));
    if(offset > ELEMENT_SIZE)
        throw std::out_of_range(std::format(
            "Offset {} is out of valid range (0~{})",
            offset, ELEMENT_SIZE));
    return Util::add(mem, ELEMENT_SIZE*index+offset);
}
const void* DynamicVector::operator[](Index index) const noexcept{
    [[likely]] if(index < memSize)
        return Util::add(mem, ELEMENT_SIZE*index);
    return nullptr;
}
void* DynamicVector::operator[](Index index) noexcept{
    [[likely]] if(index < memSize)
        return Util::add(mem, ELEMENT_SIZE*index);
    return nullptr;
}
const void* DynamicVector::data() const noexcept{ return mem; }
void* DynamicVector::data() noexcept{ return mem; }

using Iterator = DynamicVector::Iterator;
using ConstIt = DynamicVector::ConstIterator;

Iterator DynamicVector::begin(){
    if(freeIndexes.size()==0 || *freeIndexes.begin()!=0)
        return Iterator(mem, ELEMENT_SIZE,
            0, usedSize,
            freeIndexes.cbegin(), freeIndexes.cend());
    Index start = 0;
    auto it = freeIndexes.cbegin();
    while(it!=freeIndexes.cend() && start != *it){
        ++start;
        ++it;
    }
    return Iterator(mem, ELEMENT_SIZE,
        0, usedSize,
        freeIndexes.cbegin(), freeIndexes.cend()
    );
}
Iterator DynamicVector::begin(Index i){
    return Iterator(mem, ELEMENT_SIZE,
        i, usedSize,
        freeIndexes.lower_bound(i), freeIndexes.cend());
}
Iterator DynamicVector::end(){
    return Iterator(mem, ELEMENT_SIZE,
        usedSize, usedSize,
        freeIndexes.find(usedSize), freeIndexes.cend());
}
Iterator::operator ConstIt(){
    return ConstIt(ptr, STRIDE,
        index, indexEnd,
        it, it_end);
}

ConstIt DynamicVector::begin() const{
    return static_cast<ConstIt>(
        const_cast<DynamicVector*>(this)->begin()
    );
}
ConstIt DynamicVector::begin(Index i) const{
    return static_cast<ConstIt>(
        const_cast<DynamicVector*>(this)->begin(i)
    );
}
ConstIt DynamicVector::end() const{
    return static_cast<ConstIt>(
        const_cast<DynamicVector*>(this)->end()
    );
}

ConstIt DynamicVector::cbegin() const{
    return begin();
}
ConstIt DynamicVector::cbegin(Index i) const{
    return begin(i);
}
ConstIt DynamicVector::cend() const{
    return end();
}

Iterator::Iterator(void* ptr, size_t STRIDE,
    Index index, Index usedSize,
    std::set<size_t>::const_iterator it,
    std::set<size_t>::const_iterator it_end)
:ptr(ptr), STRIDE(STRIDE),
index(index), indexEnd(usedSize),
it(it), it_end(it_end){}

void* Iterator::operator*(){
    return Util::add(ptr, STRIDE*index);
}
const void* Iterator::operator*() const{
    return Util::add(ptr, STRIDE*index);
}
Iterator& Iterator::operator++(){
    assert(index < indexEnd);
    ++index;
    if(it == it_end)
        return *this;
    if(index==indexEnd)
        return *this;
    if(index < *it)
        return *this;

    while(index != *it){
        ++index;
        ++it;
    }
    assert(index <= indexEnd);
    return *this;
}
bool Iterator::operator!=(const Iterator& other) const{
    return (ptr != other.ptr) || (index != other.index);
}
bool Iterator::operator==(const Iterator& other) const{
    return (ptr == other.ptr) && (index == other.index);
}

ConstIt::ConstIterator(const void* ptr, size_t STRIDE,
    Index index, Index usedSize,
    std::set<size_t>::const_iterator it,
    std::set<size_t>::const_iterator it_end)
:ptr(ptr), STRIDE(STRIDE),
index(index), indexEnd(usedSize),
it(it), it_end(it_end){}

const void* ConstIt::operator*() const{ return Util::add(ptr, STRIDE*index); }
ConstIt& ConstIt::operator++(){
    assert(index < indexEnd);
    ++index;
    if(it == it_end)
        return *this;
    if(index==indexEnd)
        return *this;
    if(index < *it)
        return *this;

    while(index != *it){
        ++index;
        ++it;
    }
    assert(index <= indexEnd);
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

size_t DynamicVector::empty() const noexcept{
    return usedSize==0;
}
size_t DynamicVector::size() const noexcept{
    assert(usedSize+freeIndexes.size()==memSize);
    return usedSize;
}
size_t DynamicVector::elmSize() const noexcept{
    return ELEMENT_SIZE;
}
void DynamicVector::reserve(size_t minCap) noexcept{
    if(memSize >= minCap)
        return;
    size_t newAllocSize = std::bit_ceil(minCap);
    if(numElement == 0){
        mem = malloc(ELEMENT_SIZE*newAllocSize);
    }
    else
        mem = realloc(mem, ELEMENT_SIZE*newAllocSize);
    memSize = newAllocSize;
    for(Index i=memSize; i<newAllocSize; ++i)
        freeIndexes.insert(i);
}
size_t DynamicVector::capacity() const noexcept{
    assert(usedSize+freeIndexes.size()==memSize);
    return memSize;
}

void DynamicVector::clear() noexcept{
    numElement = 0;
    usedSize = 0;
    auto hint = freeIndexes.end();
    for(Index i=0; i<memSize; ++i)
        hint = freeIndexes.emplace_hint(hint, i);
}

Index DynamicVector::reserveFreeIndex() noexcept{
    if(freeIndexes.size() == 0)
        reserve(memSize + 1);
    Index freeIndex = *freeIndexes.cbegin();
    freeIndexes.erase(freeIndex);
    return freeIndex;
}

void DynamicVector::remove(Index pos, size_t num){
    if(memSize < pos+num)
        throw std::out_of_range(std::format(
            "Index {}~{} is out of valid range.",
            memSize, pos+num));
    if(pos+num < 0)
        throw std::out_of_range(std::format(
            "Index {}~{} is out of valid range.",
            pos+num, 0));
    for(Index i=pos; i<pos+num; ++i){
        auto [_, emplaced] = freeIndexes.emplace(i);
        if(emplaced)
            numElement -= 1;
    }
    assert(numElement + freeIndexes.size() == memSize);
    if(usedSize <= pos+num)
        usedSize = pos;
}











Index DynamicVector::newChunk(size_t numChunk){
    assert(numChunk > 0);
    Index start = findContinuousFreeFittedSlot(numChunk);
    // not have free slot.
    if(start == std::numeric_limits<size_t>::max()){
        size_t newAllocSize = std::bit_ceil(usedSize+numChunk);
        assert(memSize < newAllocSize);
        if(numElement == 0)
            mem = malloc(ELEMENT_SIZE*newAllocSize);
        else
            mem = realloc(mem, ELEMENT_SIZE*newAllocSize);
        for(Index i=usedSize; i<memSize; ++i)
            freeIndexes.erase(i);
        memSize = newAllocSize;
        start = usedSize;
        numElement += numChunk;
        usedSize += numChunk;
        for(Index i=start+numChunk; i<newAllocSize; ++i)
            freeIndexes.insert(i);
        return start;
    }

    for(Index i=0; i<numChunk; ++i){
        freeIndexes.erase(start+i);
    }
    numElement += numChunk;
    usedSize = std::max(usedSize, start+numChunk);
    return start;
}

Index DynamicVector::findContinuousFreeFittedSlot(
    size_t numChunk
){
    assert((numElement+freeIndexes.size())==memSize);
    if(memSize-numElement < numChunk)
        return std::numeric_limits<size_t>::max();

    Index candidateStart = *freeIndexes.begin();;
    size_t numAvailable = 1;

    for(Index freeIndex: freeIndexes){
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
    assert(numChunk <= numElement);
    for(Index i=0; i<numChunk; ++i){
        freeIndexes.insert(start+i);
    }
    numElement -= numChunk;
    assert(start+numChunk<=usedSize);
    if((start+numChunk) >= usedSize){
        for(auto it=freeIndexes.rbegin(); it!=freeIndexes.rend(); ++it){
            if(usedSize > *it + 1)
                break;
            else if(usedSize - *it == 1)
                --usedSize;
        }
    }
}
