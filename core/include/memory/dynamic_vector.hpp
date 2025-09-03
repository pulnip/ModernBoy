#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cassert>
#include <cstdlib>
#include <format>
#include <optional>
#include <set>
#include <stdexcept>
#include <utility>
#include "alias.hpp"
#include "concepts.hpp"
#include "memory/bit.hpp"

namespace ModernBoy{
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

    class DynamicVector{
    private:
        void* mem = nullptr;
        const size_t CHUNK_SIZE;
        size_t size_ = 0;
        size_t cap_ = 0;

    public:
        DynamicVector() = delete;
        ~DynamicVector();
        DynamicVector(size_t CHUNK_SIZE);
        DynamicVector(size_t CHUNK_SIZE, size_t initial_size);
        DynamicVector(const DynamicVector&) = delete;
        DynamicVector(DynamicVector&&) = delete;
        DynamicVector& operator=(const DynamicVector&) = delete;
        DynamicVector& operator=(DynamicVector&&) = delete;

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