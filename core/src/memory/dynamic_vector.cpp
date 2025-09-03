#include <algorithm>
#include <cassert>
#include <bit>
#include <iostream>
#include <limits>
#include "memory/dynamic_vector.hpp"

using namespace ModernBoy;

DynamicVector::~DynamicVector(){
    if(mem != nullptr)
        free(mem);
}
DynamicVector::DynamicVector(size_t CHUNK_SIZE)
:CHUNK_SIZE(CHUNK_SIZE){}
DynamicVector::DynamicVector(size_t CHUNK_SIZE, size_t initial_cap)
:CHUNK_SIZE(CHUNK_SIZE), cap_(initial_cap){
    if(CHUNK_SIZE != 0 && initial_cap != 0)
        mem = malloc(CHUNK_SIZE*initial_cap);
}

DynamicVector::Iterator::Iterator(void* mem, size_t CHUNK_SIZE, Index pos)
:mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
void* DynamicVector::Iterator::operator*(){
    return Util::add(mem, CHUNK_SIZE*pos);
}
const void* DynamicVector::Iterator::operator*() const{
    assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
    return Util::add(mem, CHUNK_SIZE*pos);
}
DynamicVector::Iterator& DynamicVector::Iterator::operator++(){
    ++pos;
    return *this;
}
bool DynamicVector::Iterator::operator!=(const DynamicVector::Iterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVector::Iterator::operator==(const DynamicVector::Iterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
bool DynamicVector::Iterator::operator!=(const DynamicVector::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVector::Iterator::operator==(const DynamicVector::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
DynamicVector::ConstIterator::ConstIterator(void* mem, size_t CHUNK_SIZE, Index pos)
:mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
const void* DynamicVector::ConstIterator::operator*(){
    return Util::add(mem, CHUNK_SIZE*pos);
}
const void* DynamicVector::ConstIterator::operator*() const{
    assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
    return Util::add(mem, CHUNK_SIZE*pos);
}
DynamicVector::ConstIterator& DynamicVector::ConstIterator::operator++(){
    ++pos;
    return *this;
}
bool DynamicVector::ConstIterator::operator!=(const DynamicVector::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVector::ConstIterator::operator==(const DynamicVector::ConstIterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}
bool DynamicVector::ConstIterator::operator!=(const DynamicVector::Iterator& other) const{
    assert(mem == other.mem);
    return pos != other.pos;
}
bool DynamicVector::ConstIterator::operator==(const DynamicVector::Iterator& other) const{
    assert(mem == other.mem);
    return pos == other.pos;
}

void* DynamicVector::operator[](Index index){
    assert(index < size_);
    assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
    return Util::add(mem, CHUNK_SIZE*index);
}
const void* DynamicVector::operator[](Index index) const{
    assert(index < size_);
    assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
    return Util::add(mem, CHUNK_SIZE*index);
}
DynamicVector::Iterator DynamicVector::begin(){
    return DynamicVector::Iterator(mem, CHUNK_SIZE, 0);
}
DynamicVector::Iterator DynamicVector::end(){
    return DynamicVector::Iterator(mem, CHUNK_SIZE, size_);
}
DynamicVector::ConstIterator DynamicVector::begin() const{
    return DynamicVector::ConstIterator(mem, CHUNK_SIZE, 0);
}
DynamicVector::ConstIterator DynamicVector::end() const{
    return DynamicVector::ConstIterator(mem, CHUNK_SIZE, size_);
}
DynamicVector::ConstIterator DynamicVector::cbegin() const{
    return DynamicVector::ConstIterator(mem, CHUNK_SIZE, 0);
}
DynamicVector::ConstIterator DynamicVector::cend() const{
    return DynamicVector::ConstIterator(mem, CHUNK_SIZE, size_);
}

size_t DynamicVector::size() const{ return size_; }
size_t DynamicVector::capacity() const{ return cap_; }
void DynamicVector::resize(size_t new_size){
    if(new_size > cap_){
        reserve(std::bit_ceil(new_size));
    }
    size_ = new_size;
}

void DynamicVector::reserve(size_t new_cap){
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

void DynamicVector::clear(){
    size_ = 0;
}

void DynamicVector::swap_remove(Index index){
    assert(index < size_ && "swap_remove out of range");
    if(index < size_ - 1 && CHUNK_SIZE > 0)
        memcpy((*this)[index], (*this)[size_-1], CHUNK_SIZE);
    --size_;
}