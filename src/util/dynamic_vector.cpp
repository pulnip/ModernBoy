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
DynamicVectorElementBadCast::DynamicVectorElementBadCast(
    size_t expected, size_t requested
):msg(std::format("bad cast: expected={}bytes, requested={}bytes", expected, requested)){}
const char* DynamicVectorElementBadCast::what(
) const noexcept{ return msg.c_str(); }

DynamicVectorElementWrapper::DynamicVectorElementWrapper(
    void* elmMem, size_t ELM_SIZE)
:elmMem(elmMem),ELM_SIZE(ELM_SIZE){}
DynamicVectorConstElementWrapper::DynamicVectorConstElementWrapper(
    const void* elmMem, size_t ELM_SIZE)
:elmMem(elmMem),ELM_SIZE(ELM_SIZE){}

DynamicVector::DynamicVector(size_t ELEMENT_SIZE) noexcept
:ELEMENT_SIZE(ELEMENT_SIZE){}

DynamicVector::DynamicVector(size_t ELEMENT_SIZE, size_t initialSize) noexcept
:mem(malloc(ELEMENT_SIZE*std::bit_ceil(initialSize))),
ELEMENT_SIZE(ELEMENT_SIZE), numElement(initialSize),
memSize(std::bit_ceil(initialSize)), usedSize(initialSize){
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
    return ConstIt(mem, ELEMENT_SIZE,
        0, usedSize,
        freeIndexes.cbegin(), freeIndexes.cend()
    );
}
ConstIt DynamicVector::end() const{
    return ConstIt(mem, ELEMENT_SIZE,
        usedSize, usedSize,
        freeIndexes.find(usedSize), freeIndexes.cend());
}

ConstIt DynamicVector::cbegin() const{
    return begin();
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

DynamicVectorElementWrapper Iterator::operator*(){
    return {Util::add(ptr, STRIDE*index), STRIDE};
}
DynamicVectorConstElementWrapper Iterator::operator*() const{
    return {Util::add(ptr, STRIDE*index), STRIDE};
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

DynamicVectorConstElementWrapper ConstIt::operator*() const{
    return {Util::add(ptr, STRIDE*index), STRIDE};
}

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

Index DynamicVector::insertRange(size_t numElm){
    assert(numElm > 0);
    Index start = findContinuousFreeFittedSlot(numElm);
    // not have free slot.
    if(start == std::numeric_limits<size_t>::max()){
        size_t newAllocSize = std::bit_ceil(usedSize+numElm);
        assert(memSize < newAllocSize);
        if(numElement == 0)
            mem = malloc(ELEMENT_SIZE*newAllocSize);
        else
            mem = realloc(mem, ELEMENT_SIZE*newAllocSize);
        for(Index i=usedSize; i<memSize; ++i)
            freeIndexes.erase(i);
        memSize = newAllocSize;
        start = usedSize;
        numElement += numElm;
        usedSize += numElm;
        for(Index i=start+numElm; i<newAllocSize; ++i)
            freeIndexes.insert(i);
        return start;
    }

    for(Index i=0; i<numElm; ++i){
        freeIndexes.erase(start+i);
    }
    numElement += numElm;
    usedSize = std::max(usedSize, start+numElm);
    return start;
}

Index DynamicVector::findContinuousFreeFittedSlot(
    size_t numElm
){
    assert((numElement+freeIndexes.size())==memSize);
    if(memSize-numElement < numElm)
        return std::numeric_limits<size_t>::max();

    Index candidateStart = *freeIndexes.begin();;
    size_t numAvailable = 1;

    for(Index freeIndex: freeIndexes){
        // Still Continuous
        if((freeIndex-candidateStart+1) == numAvailable){
            // find Continuous Chunk!
            if(numAvailable == numElm){
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
    if(usedSize )
    if(usedSize >= pos+num)
        for(auto it=freeIndexes.rbegin(); it!=freeIndexes.rend(); ++it){
            if(usedSize > *it + 1)
                break;
            else if(usedSize - *it == 1)
                --usedSize;
        }
}


DynamicVectorV2::~DynamicVectorV2(){
    std::println("DynamicVectorV2 size: {}, capacity: {}", size_, cap_);
    if(mem != nullptr)
        free(mem);
}
DynamicVectorV2::DynamicVectorV2(size_t CHUNK_SIZE)
:CHUNK_SIZE(CHUNK_SIZE){}
DynamicVectorV2::DynamicVectorV2(size_t CHUNK_SIZE, size_t initial_cap)
:CHUNK_SIZE(CHUNK_SIZE), cap_(initial_cap){
    if(CHUNK_SIZE != 0 && initial_cap != 0)
        mem = malloc(CHUNK_SIZE*initial_cap);
}

DynamicVectorV2::Iterator::Iterator(void* mem, size_t CHUNK_SIZE, Index pos)
:mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
void* DynamicVectorV2::Iterator::operator*(){
    return Util::add(mem, CHUNK_SIZE*pos);
}
const void* DynamicVectorV2::Iterator::operator*() const{
    assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
    return Util::add(mem, CHUNK_SIZE*pos);
}
DynamicVectorV2::Iterator& DynamicVectorV2::Iterator::operator++(){
    ++pos;
    return *this;
}
bool DynamicVectorV2::Iterator::operator!=(const DynamicVectorV2::Iterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVectorV2::Iterator::operator==(const DynamicVectorV2::Iterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
bool DynamicVectorV2::Iterator::operator!=(const DynamicVectorV2::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVectorV2::Iterator::operator==(const DynamicVectorV2::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
DynamicVectorV2::ConstIterator::ConstIterator(void* mem, size_t CHUNK_SIZE, Index pos)
:mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
const void* DynamicVectorV2::ConstIterator::operator*(){
    return Util::add(mem, CHUNK_SIZE*pos);
}
const void* DynamicVectorV2::ConstIterator::operator*() const{
    assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
    return Util::add(mem, CHUNK_SIZE*pos);
}
DynamicVectorV2::ConstIterator& DynamicVectorV2::ConstIterator::operator++(){
    ++pos;
    return *this;
}
bool DynamicVectorV2::ConstIterator::operator!=(const DynamicVectorV2::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVectorV2::ConstIterator::operator==(const DynamicVectorV2::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
bool DynamicVectorV2::ConstIterator::operator!=(const DynamicVectorV2::Iterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVectorV2::ConstIterator::operator==(const DynamicVectorV2::Iterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}

void* DynamicVectorV2::operator[](Index index){
    assert(index < size_);
    assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
    return Util::add(mem, CHUNK_SIZE*index);
}
const void* DynamicVectorV2::operator[](Index index) const{
    assert(index < size_);
    assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
    return Util::add(mem, CHUNK_SIZE*index);
}
DynamicVectorV2::Iterator DynamicVectorV2::begin(){
    return DynamicVectorV2::Iterator(mem, CHUNK_SIZE, 0);
}
DynamicVectorV2::Iterator DynamicVectorV2::end(){
    return DynamicVectorV2::Iterator(mem, CHUNK_SIZE, size_);
}
DynamicVectorV2::ConstIterator DynamicVectorV2::begin() const{
    return DynamicVectorV2::ConstIterator(mem, CHUNK_SIZE, 0);
}
DynamicVectorV2::ConstIterator DynamicVectorV2::end() const{
    return DynamicVectorV2::ConstIterator(mem, CHUNK_SIZE, size_);
}
DynamicVectorV2::ConstIterator DynamicVectorV2::cbegin() const{
    return DynamicVectorV2::ConstIterator(mem, CHUNK_SIZE, 0);
}
DynamicVectorV2::ConstIterator DynamicVectorV2::cend() const{
    return DynamicVectorV2::ConstIterator(mem, CHUNK_SIZE, size_);
}

size_t DynamicVectorV2::size() const{ return size_; }
size_t DynamicVectorV2::capacity() const{ return cap_; }
void DynamicVectorV2::resize(size_t new_size){
    if(new_size > cap_){
        reserve(std::bit_ceil(new_size));
    }
    size_ = new_size;
}

void DynamicVectorV2::reserve(size_t new_cap){
    if(new_cap <= cap_)
        return;

    if(CHUNK_SIZE != 0){
        auto new_mem = realloc(mem, CHUNK_SIZE*new_cap);
        if(new_mem == nullptr)
            throw std::runtime_error("realloc failed!");
        mem = new_mem;
    }
    cap_ = new_cap;
    assert(cap_ >= size_);
}

void DynamicVectorV2::clear(){
    size_ = 0;
}

void DynamicVectorV2::swap_remove(Index index){
    assert(index < size_ && "swap_remove out of range");
    if(index < size_ - 1 && CHUNK_SIZE > 0)
        std::memmove((*this)[index], (*this)[size_-1], CHUNK_SIZE);
    --size_;
}