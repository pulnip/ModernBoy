#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cstdlib>
#include <format>
#include <set>
#include <stdexcept>
#include <type_traits>
#include "common/alias.hpp"
#include "util/bit.hpp"

namespace ModernBoy{
    class DynamicVectorBadCast: public std::bad_cast{
        std::string msg;

    public:
        DynamicVectorBadCast(size_t expected, size_t requested);
        const char* what() const noexcept override;  
    };

    class DynamicVector{
    private:
        void* mem = nullptr;
        const size_t ELEMENT_SIZE;
        size_t numElement = 0;
        size_t memSize = 0;
        size_t usedSize = 0;
        std::set<size_t> freeIndexes{};

    public:
        DynamicVector(size_t ELEMENT_SIZE) noexcept;
        DynamicVector(size_t ELEMENT_SIZE, size_t initialSize) noexcept;
        DynamicVector() = delete;
        ~DynamicVector();
        DynamicVector(const DynamicVector&)=delete;
        DynamicVector(DynamicVector&& other);
        DynamicVector& operator=(const DynamicVector& other)=delete;
        DynamicVector& operator=(DynamicVector&& other);

        const void* at(Index index) const;
        const void* at(Index index, size_t offset) const;
        void* at(Index index);
        void* at(Index index, size_t offset);
        template<typename T>
        const T& at(Index index) const{
            if(sizeof(T) != ELEMENT_SIZE)
                throw DynamicVectorBadCast(ELEMENT_SIZE, sizeof(T));
            if(index >= memSize)
                throw std::out_of_range(std::format(
                    "Index {} is not inserted before",
                    index));
            return Util::add(mem, ELEMENT_SIZE*index);
        }
        template<typename T>
        const T& at(Index index, size_t offset) const{
            if(sizeof(T) > ELEMENT_SIZE)
                throw DynamicVectorBadCast(ELEMENT_SIZE, sizeof(T));
            if(index >= memSize || freeIndexes.contains(index))
                throw std::out_of_range(std::format(
                    "Index {} is not inserted before",
                    index));
            if(offset+sizeof(T) > ELEMENT_SIZE)
                throw std::out_of_range(std::format(
                    "Offset {} is out of valid range (0~{})",
                    offset, ELEMENT_SIZE - sizeof(T)));
            return *static_cast<std::remove_cvref_t<T>*>(
                Util::add(mem, ELEMENT_SIZE*index+offset));
        }
        template<typename T>
        T& at(Index index){
            if(sizeof(T) != ELEMENT_SIZE)
                throw DynamicVectorBadCast(ELEMENT_SIZE, sizeof(T));
            if(index >= memSize)
                throw std::out_of_range(std::format(
                    "Index {} is not inserted before",
                    index));
            return Util::add(mem, ELEMENT_SIZE*index);
        }
        template<typename T>
        T& at(Index index, size_t offset){
            if(sizeof(T) > ELEMENT_SIZE)
                throw DynamicVectorBadCast(ELEMENT_SIZE, sizeof(T));
            if(index >= memSize || freeIndexes.contains(index))
                throw std::out_of_range(std::format(
                    "Index {} is not inserted before",
                    index));
            if(offset+sizeof(T) > ELEMENT_SIZE)
                throw std::out_of_range(std::format(
                    "Offset {} is out of valid range (0~{})",
                    offset, ELEMENT_SIZE - sizeof(T)));
            return *static_cast<std::remove_cvref_t<T>*>(
                Util::add(mem, ELEMENT_SIZE*index+offset));
        }
        template<typename T>
        void set(Index index, size_t offset, T&& val){
            at<T>(index, offset) = std::forward<T>(val);
        }
        template<typename T1, typename... TN>
        void set(Index index, size_t offset, T1&& val, TN&&... args){
            set(index, offset+sizeof(T1), std::forward<TN>(args)...);
            at<T1>(index, offset) = std::forward<T1>(val);
        }
        template<typename T>
        void get(Index index, size_t offset, T& val) const{
            val = at<T>(index, offset);
        }
        template<typename T1, typename... TN>
        void get(Index index, size_t offset, T1& val, TN&... args) const{
            get<TN...>(index, offset+sizeof(T1), args...);
            val = at<T1>(index, offset);
        }
        const void* operator[](Index index) const noexcept;
        void* operator[](Index index) noexcept;
        const void* data() const noexcept;
        void* data() noexcept;

        struct ConstIterator{
            const void* const ptr;
            const size_t STRIDE;
            Index index;
            const Index indexEnd;
            std::set<size_t>::const_iterator it;
            const std::set<size_t>::const_iterator it_end;

            ConstIterator(const void* ptr, size_t STRIDE,   
                Index index, Index indexEnd, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);

            const void* operator*() const;
            ConstIterator& operator++();
            bool operator!=(const ConstIterator& other) const;
            bool operator==(const ConstIterator& other) const;
        };
        struct Iterator{
            void* const ptr;
            const size_t STRIDE;
            Index index;
            const Index indexEnd;
            std::set<size_t>::const_iterator it;
            const std::set<size_t>::const_iterator it_end;

            Iterator(void* ptr, size_t STRIDE,
                Index index, Index indexEnd, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);

            operator ConstIterator();

            void* operator*();
            const void* operator*() const;
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            bool operator==(const Iterator& other) const;
        };

        Iterator begin();
        Iterator begin(Index i);
        Iterator end();
        ConstIterator begin() const;
        ConstIterator begin(Index i) const;
        ConstIterator end() const;
        ConstIterator cbegin() const;
        ConstIterator cbegin(Index i) const;
        ConstIterator cend() const;

        size_t empty() const noexcept;
        size_t size() const noexcept;
        size_t elmSize() const noexcept;
        void reserve(size_t minCap) noexcept;
        size_t capacity() const noexcept;

        void clear() noexcept;
        Index insertRange(size_t num);
        template<typename T>
        Index emplace(T&& val){
            if(sizeof(T) != ELEMENT_SIZE)
                throw DynamicVectorBadCast(ELEMENT_SIZE, sizeof(T));
            Index reservedIndex = insertRange(1);
            (*this)[reservedIndex] = std::move(val);
        }
        void remove(Index pos, size_t num=1);

    private:
        size_t checkSize(size_t elmSize) const;

        void moveFrom(DynamicVector&& other);
        Index findContinuousFreeFittedSlot(size_t numChunk);
    };

    bool operator==(const DynamicVector::ConstIterator& lhs, const DynamicVector::Iterator& rhs);
    bool operator==(const DynamicVector::Iterator& lhs, const DynamicVector::ConstIterator& rhs);
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP