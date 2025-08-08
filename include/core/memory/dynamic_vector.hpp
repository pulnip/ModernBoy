#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cassert>
#include <cstdlib>
#include <format>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include "core/alias.hpp"
#include "core/concepts.hpp"
#include "core/memory/bit.hpp"

namespace ModernBoy{
    class DynamicVectorBadCast: public std::bad_cast{
        std::string msg;

    public:
        DynamicVectorBadCast(size_t expected, size_t requested);
        const char* what() const noexcept override;  
    };
    class DynamicVectorElementBadCast: public std::bad_cast{
        std::string msg;

    public:
        DynamicVectorElementBadCast(size_t expected, size_t requested);
        const char* what() const noexcept override;  
    };

    struct DynamicVectorElementWrapper{
        DynamicVectorElementWrapper(void* elmMem,
            size_t ELM_SIZE);

        template<typename T>
        T& at(size_t offset){
            if(sizeof(T)+offset > ELM_SIZE)
                throw DynamicVectorElementBadCast(ELM_SIZE, offset + sizeof(T));
            return *static_cast<std::remove_reference_t<T>*>(
                Util::add(elmMem, ELM_SIZE*offset));
        }

    // private:
        void* elmMem = nullptr;
        const size_t ELM_SIZE;
    };
    struct DynamicVectorConstElementWrapper{
        DynamicVectorConstElementWrapper(const void* elmMem,
            size_t ELM_SIZE);

        template<typename T>
        const T& at(size_t offset) const{

            if(offset+sizeof(T) > ELM_SIZE)
                throw DynamicVectorElementBadCast(ELM_SIZE, offset + sizeof(T));
            return *static_cast<const T*>(
                Util::add(elmMem, offset));
        }

    // private:
        const void* elmMem = nullptr;
        const size_t ELM_SIZE;
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
            const void* ptr;
            size_t STRIDE;
            Index index;
            Index indexEnd;
            std::set<size_t>::const_iterator it;
            std::set<size_t>::const_iterator it_end;

            ConstIterator(const void* ptr, size_t STRIDE,   
                Index index, Index indexEnd, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);
            ConstIterator(const ConstIterator&) = default;
            ConstIterator(ConstIterator&&) = default;
            ConstIterator& operator=(const ConstIterator&) = default;
            ConstIterator& operator=(ConstIterator&&) = default;

            // DynamicVectorConstElementWrapper wrapped() const;
            DynamicVectorConstElementWrapper operator*() const;
            ConstIterator& operator++();
            bool operator!=(const ConstIterator& other) const;
            bool operator==(const ConstIterator& other) const;
        };
        struct Iterator{
            void* ptr;
            size_t STRIDE;
            Index index;
            Index indexEnd;
            std::set<size_t>::const_iterator it;
            std::set<size_t>::const_iterator it_end;

            Iterator(void* ptr, size_t STRIDE,
                Index index, Index indexEnd, 
                std::set<size_t>::const_iterator it,
                std::set<size_t>::const_iterator it_end);
            Iterator(const Iterator&) = default;
            Iterator(Iterator&&) = default;
            Iterator& operator=(const Iterator&) = default;
            Iterator& operator=(Iterator&&) = default;

            operator ConstIterator();

            // DynamicVectorElementWrapper wrapped() const;
            DynamicVectorElementWrapper operator*();
            DynamicVectorConstElementWrapper operator*() const;
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            bool operator==(const Iterator& other) const;
        };

        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;
        ConstIterator cbegin() const;
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

    template<ValueType T>
    void emplace(void* dst, T&& t){
        *static_cast<T*>(dst) = std::forward<T>(t);
    }
    template<ValueType T1, AllValue... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn){
        *static_cast<T1*>(dst) = std::forward<T1>(t1);
        emplace(Util::add<T1>(dst), std::forward<TN>(tn)...);
    }
    template<PointerType T>
    void emplace(void* dst, const T t){
        using U = std::remove_pointer_t<std::remove_cvref_t<T>>;

        if constexpr(!std::is_null_pointer_v<U>)
            *static_cast<U*>(dst) = *t;
    }
    template<PointerType T1, AllPointer... TN>
    void emplace(void* dst, const T1 t1, const TN... tn){
        using U = std::remove_pointer_t<std::remove_cvref_t<T1>>;

        if constexpr(!std::is_null_pointer_v<T1>){
            *static_cast<U*>(dst) = *t1;
            dst = Util::add<U>(dst);
        }
        emplace(dst, tn...);
    }
    template<OptionalType T>
    void emplace(void* dst, T&& t){
        using U = remove_optional_t<std::remove_cvref_t<T>>;

        if(t.has_value())
            *static_cast<U*>(dst) = t.value();
    }
    template<OptionalType T1, AllOptional... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn){
        using U = remove_optional_t<std::remove_cvref_t<T1>>;

        if(t1.has_value()){
            *static_cast<U*>(dst) = t1.value();
            dst = Util::add<U>(dst);
        }
        emplace(dst, std::forward<TN>(tn)...);
    }

    template<typename... T>
    constexpr size_t sum_sizeof(){
        return (size_t{0} + ... + sizeof(T));
    }

    class DynamicVectorV2{
    private:
        void* mem = nullptr;
        const size_t CHUNK_SIZE;
        size_t size_ = 0;
        size_t cap_ = 0;

    public:
        DynamicVectorV2() = delete;
        ~DynamicVectorV2();
        DynamicVectorV2(size_t CHUNK_SIZE);
        DynamicVectorV2(size_t CHUNK_SIZE, size_t initial_size);
        DynamicVectorV2(const DynamicVector&) = delete;
        DynamicVectorV2(DynamicVector&&) = delete;
        DynamicVectorV2& operator=(const DynamicVectorV2&) = delete;
        DynamicVectorV2& operator=(DynamicVector&&) = delete;

        class ConstIterator;

        class Iterator{
        private:
            void* const mem;
            const size_t CHUNK_SIZE;
            Index pos;

            friend ConstIterator;

        public:
            Iterator(void* mem, size_t CHUNK_SIZE, Index pos);
            Iterator(const Iterator&) = default;
            Iterator(Iterator&&) = default;
            Iterator& operator=(const Iterator&) = delete;
            Iterator& operator=(Iterator&&) = delete;

            void* operator*();
            const void* operator*() const;
            Iterator& operator++();
            bool operator!=(const Iterator& other) const;
            bool operator==(const Iterator& other) const;
            bool operator!=(const ConstIterator& other) const;
            bool operator==(const ConstIterator& other) const;
        };
        class ConstIterator{
        private:
            void* mem;
            size_t CHUNK_SIZE;
            Index pos;

            friend Iterator;

        public:
            ConstIterator(void* mem, size_t CHUNK_SIZE, Index pos);
            ConstIterator(const ConstIterator&) = default;
            ConstIterator(ConstIterator&&) = default;
            ConstIterator& operator=(const ConstIterator&) = default;
            ConstIterator& operator=(ConstIterator&&) = default;

            const void* operator*();
            const void* operator*() const;
            ConstIterator& operator++();
            bool operator!=(const ConstIterator& other) const;
            bool operator==(const ConstIterator& other) const;
            bool operator!=(const Iterator& other) const;
            bool operator==(const Iterator& other) const;
        };

        void* operator[](Index index);
        const void* operator[](Index index) const;
        Iterator begin();
        Iterator end();
        ConstIterator begin() const;
        ConstIterator end() const;
        ConstIterator cbegin() const;
        ConstIterator cend() const;

        size_t size() const;
        size_t capacity() const;
        void resize(size_t new_size);
        void reserve(size_t new_cap);
        void clear();

        template<AllValue... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_reference_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... + sizeof(T));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = Util::add(mem, (size_-1)*CHUNK_SIZE);
            ModernBoy::emplace(dst, std::forward<T>(t)...);
        }
        template<AllPointer... T>
        void emplace(const T... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_pointer_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (std::is_null_pointer_v<T> ? 0 : sizeof(std::remove_pointer_t<T>)));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = Util::add(mem, (size_-1)*CHUNK_SIZE);
            ModernBoy::emplace(dst, t...);
        }
        template<AllOptional... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<remove_optional_t<std::remove_cvref_t<T>>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (t.has_value() ? sizeof(remove_optional_t<std::remove_cvref_t<T>>) : 0));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = Util::add(mem, (size_-1)*CHUNK_SIZE);
            ModernBoy::emplace(dst, std::forward<T>(t)...);
        }

        void swap_remove(Index index);
    };
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP