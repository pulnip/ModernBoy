#ifndef MODERNBOY_DYNAMIC_VECTOR_HPP
#define MODERNBOY_DYNAMIC_VECTOR_HPP

#include <cstdlib>
#include <set>
#include <type_traits>

namespace ModernBoy{
    using Index = size_t;
    using Byte = size_t;

    class DynamicVector{
    private:
        size_t CHUNK_SIZE;
        size_t maxSize = 0;
        void* data = nullptr;
        size_t size = 0;
        std::set<size_t> freeSlots{};
    #ifdef _DEBUG
        size_t numChunk_last=0;
    #endif

    public:
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

    private:
        void moveFrom(DynamicVector&& other);
        Index findContinuousFreeFittedSlot(size_t numChunk);
    };

    void* getChunkData(void* chunk, Byte offset);
    void setChunkData(const void* data, void* chunk, Byte offset);

    template<typename T>
    T& getChunkData_(void* chunk, Byte offset){
        return *reinterpret_cast<T*>(
            static_cast<uint8_t*>(chunk)+offset
        );
    }
    template<typename T>
    void setChunkData_(const T& data, void* chunk, Byte offset){
        *reinterpret_cast<T*>(
            static_cast<uint8_t*>(chunk)+offset
        ) = data;
    }
    template<typename T>
    void setChunkData_(T&& data, void* chunk, Byte offset){
        *reinterpret_cast<T*>(
            static_cast<uint8_t*>(chunk)+offset
        ) = std::move(data);
    }
    template<typename T1, typename... TN>
    consteval size_t size_of(){
        return sizeof(T1)+size_of<TN...>();
    }
    template<typename T1>
    consteval size_t size_of(){
        return sizeof(T1);
    }

    template<typename D, typename T1, typename... TN>
    constexpr void setChunkData__(const D& data, void* chunkMem){
        if constexpr(std::same_as<D, T1>)
            memcpy(chunkMem, &data, sizeof(D));
        else
            setChunkData__<D, TN...>(data,
                static_cast<char*>(chunkMem)+sizeof(T1));
    }
    template<typename D, typename T1>
        requires std::same_as<D, T1>
    constexpr void setChunkData__(const D& data, void* chumkMem){
        memcpy(chumkMem, &data, sizeof(D));
    }
    template<typename D, typename T1, typename... TN>
    D& getChunkData__(void* chunkMem){
        if constexpr(std::same_as<D, T1>)
            return *static_cast<D*>(chunkMem);
        else
            getChunkData__<D, TN...>(
                static_cast<char*>(chunkMem)+sizeof(T1));
    }
    template<typename D, typename T1>
        requires std::same_as<D, T1>
    D& getChunkData__(void* chunkMem){
        return *static_cast<D*>(chunkMem);
    }
} // namespace ModernBoy

#endif // MODERNBOY_DYNAMIC_VECTOR_HPP