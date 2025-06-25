#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cstdlib>
#include <set>
#include <type_traits>
#include "common/alias.hpp"
#include "util/bit.hpp"

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
            void* const ptr;
            Index index;
            const size_t STRIDE;
            std::set<size_t>::const_iterator it;

            Iterator(void* ptr, Index index, size_t STRIDE,
                std::set<size_t>::const_iterator it);

            void* operator*();
            const void* operator*() const;
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
        };
        struct ConstIterator{
            const void* const ptr;
            Index index;
            const size_t STRIDE;
            std::set<size_t>::const_iterator it;

            ConstIterator(const void* ptr, Index index, size_t STRIDE,
                std::set<size_t>::const_iterator it);

            const void* operator*() const;
            ConstIterator& operator++();
            bool operator!=(const ConstIterator& other) const;
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

        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;
        ConstIterator cbegin() const;
        ConstIterator cend() const;

    private:
        void moveFrom(DynamicVector&& other);
        Index findContinuousFreeFittedSlot(size_t numChunk);
    };
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP