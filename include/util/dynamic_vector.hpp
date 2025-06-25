#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cstdlib>
#include <set>
#include <type_traits>
#include "common/alias.hpp"

namespace ModernBoy{
    using Byte = size_t;

    class DynamicVector{
    private:
        size_t CHUNK_SIZE;
        size_t maxSize = 0;
        void* data = nullptr;
        size_t size_ = 0;
        std::set<size_t> freeSlots{};
    #ifdef _DEBUG
        size_t numChunk_last=0;
    #endif

    public:
        struct Iterator{
            void* ptr;
            const size_t STRIDE;

            Iterator(void* ptr, size_t STRIDE)
            :ptr(ptr),STRIDE(STRIDE){}

            void* operator*(){ return ptr; }
            const void* operator*() const{ return ptr; }
            Iterator& operator++(){
                ptr = static_cast<uint8_t*>(ptr) + STRIDE;
                return *this;
            }
            bool operator!=(const Iterator& other) const{
                return ptr != other.ptr; }
        };
        struct ConstIterator{
            const void* ptr;
            const size_t STRIDE;

            ConstIterator(const void* ptr, size_t STRIDE)
            :ptr(ptr),STRIDE(STRIDE){}

            const void* operator*() const{ return ptr; }
            ConstIterator& operator++(){
                ptr = static_cast<const uint8_t*>(ptr) + STRIDE;
                return *this;
            }
            bool operator!=(const ConstIterator& other) const{
                return ptr != other.ptr; }
        };

        DynamicVector(size_t CHUNK_SIZE);
        DynamicVector(size_t CHUNK_SIZE, size_t initialSize);
        DynamicVector() = delete;
        ~DynamicVector();
        DynamicVector(DynamicVector&& other);
        DynamicVector& operator=(DynamicVector&& other);

        // Start index of Chunk(s).
        Index newChunk(size_t numChunk=1);
        void freeChunk(Index startIndex, size_t numChunk=1);
        void* operator[](Index index);
        size_t size() const noexcept;
        size_t capacity() const noexcept;

        Iterator begin(){ return Iterator(data, CHUNK_SIZE); }
        Iterator end(){ return Iterator(static_cast<uint8_t*>(data)+size_, CHUNK_SIZE); }
        ConstIterator begin() const{ return ConstIterator(data, CHUNK_SIZE); }
        ConstIterator end() const{ return ConstIterator(static_cast<uint8_t*>(data)+size_, CHUNK_SIZE); }
        ConstIterator cbegin() const{ return ConstIterator(data, CHUNK_SIZE); }
        ConstIterator cend() const{ return ConstIterator(static_cast<uint8_t*>(data)+size_, CHUNK_SIZE); }

    private:
        void moveFrom(DynamicVector&& other);
        Index findContinuousFreeFittedSlot(size_t numChunk);
    };
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP