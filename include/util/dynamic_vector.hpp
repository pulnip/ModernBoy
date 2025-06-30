#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cstdlib>
#include <set>
#include <type_traits>
#include "common/alias.hpp"
#include "util/bit.hpp"

namespace ModernBoy{
    class DynamicVector{
    private:
    void* data = nullptr;
        const size_t CHUNK_SIZE;
        size_t usedSize= 0;
        size_t allocatedSize = 0;
        size_t maxUsedSize=0;
        std::set<size_t> freeSlots{};

    public:
        struct Iterator{
            void* const ptr;
            const size_t STRIDE;
            Index index;
            const Index maxUsedSize;
            std::set<size_t>::const_iterator it;
            const std::set<size_t>::const_iterator it_end;

            Iterator(void* ptr, size_t STRIDE,
                Index index, Index maxUsedSize, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);

            void* operator*();
            const void* operator*() const;
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            bool operator==(const Iterator& other) const;
        };
        struct ConstIterator{
            const void* const ptr;
            const size_t STRIDE;
            Index index;
            const Index maxUsedSize;
            std::set<size_t>::const_iterator it;
            const std::set<size_t>::const_iterator it_end;

            ConstIterator(const void* ptr, size_t STRIDE,   
                Index index, Index maxUsedSize, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);

            const void* operator*() const;
            ConstIterator& operator++();
            bool operator!=(const ConstIterator& other) const;
            bool operator==(const ConstIterator& other) const;
        };

        DynamicVector(size_t CHUNK_SIZE);
        DynamicVector(size_t CHUNK_SIZE, size_t initialSize);
        DynamicVector() = delete;
        ~DynamicVector();
        DynamicVector(DynamicVector&& other);
        DynamicVector& operator=(DynamicVector&& other);

        Index newChunk(size_t numChunk=1);
        void freeChunk(Index startIndex, size_t numChunk=1);
        size_t getChunkSize() const;
        void* operator[](Index index);
        const void* operator[](Index index) const;
        size_t size() const noexcept;
        size_t capacity() const noexcept;
        void* raw() noexcept;
        const void* raw() const noexcept;

        Iterator begin();
        Iterator begin(Index i);
        Iterator end();
        ConstIterator begin() const;
        ConstIterator begin(Index i) const;
        ConstIterator end() const;
        ConstIterator cbegin() const;
        ConstIterator cbegin(Index i) const;
        ConstIterator cend() const;

    private:
        void moveFrom(DynamicVector&& other);
        Index findContinuousFreeFittedSlot(size_t numChunk);
    };

    bool operator==(const DynamicVector::ConstIterator& lhs, const DynamicVector::Iterator& rhs);
    bool operator==(const DynamicVector::Iterator& lhs, const DynamicVector::ConstIterator& rhs);
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP