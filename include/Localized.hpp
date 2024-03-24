#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <optional>
#include "Math.hpp"

template<typename T>
concept Initializable = requires(T t){
    t.init();
};

// pointer-like mechanism
using DataDescriptor=unsigned int;

template<Initializable T, DataDescriptor MAX_SECTION=256>
    requires(MAX_SECTION < 256*sizeof(DataDescriptor))
class DataReference;

template<Initializable T, DataDescriptor MAX_SECTION=256>
    requires(MAX_SECTION < 256*sizeof(DataDescriptor))
class Localized{
    using Ref = DataReference<T, MAX_SECTION>;
    friend Ref;
    static constexpr DataDescriptor MAX_BLOCK = sizeof(DataDescriptor) / MAX_SECTION;

  public:
    Localized(){
        for(DataDescriptor i=0; i < MAX_SECTION; ++i){
            stack[i] = i;
        }
        assert(isUnique());
    }

    bool isEmpty() const noexcept{ return head==0; }
    bool isFull() const noexcept{ return head==MAX_SECTION; }

    std::optional<Ref> allocate(bool init=false){
        assert(isUnique());
        // all memory is allocated.
        if(isFull()){
            // so, cannot return id.
            return std::nullopt;
        }

        // pop id from non-assigned section list.
        DataDescriptor result=stack[head++];
        // cannot assign already-assigned section.
        assert(not memory[result].assignFlag);

        memory[result].assignFlag = true;
        // if request init T
        if(init){
            memory[result].data.init();
        }

        return Ref(*this, result);
        // memory[poped-id] is now no longer assignable.
    }

  private:
    // stack verification algo.
    bool isUnique(){
        // allocated memory id must be unique
        bool id_exist[MAX_SECTION]={false, };
        for(auto i=head; i<MAX_SECTION; ++i){
            auto& exist=id_exist[stack[i]];
            if(exist) return false;
            exist=true;
        }

        return true;
    }

    // access only allow to Data Reference
    T& operator[](DataDescriptor id){
        // segmentation fault...
        assert(memory[id].assignFlag);
        return memory[id].data;
    }

    // free only allow to Data Reference
    void free(DataDescriptor id){
        // memory must not be empty.
        assert(not isEmpty());
        // cannot free not assigned section.
        assert(memory[id].assignFlag);

        memory[id].assignFlag=false;
        // push id to non-assigned section list.
        stack[--head]=id;

        assert(isUnique());
        // memory[pushed-id] is now assignable.
    }

  private:
    struct Section{
        bool assignFlag=false;
        T data;
    };
    struct Block{
        Section sections[MAX_SECTION];
    };
    Section memory[MAX_SECTION];


    // non-assigned section list
    DataDescriptor stack[MAX_SECTION];
    // stack[MAX_SECTION-1] is bottom
    DataDescriptor head=0;
};

template<Initializable T, DataDescriptor MAX_SECTION>
    requires(MAX_SECTION < 256*sizeof(DataDescriptor))
class DataReference{
    using Container = Localized<T, MAX_SECTION>;
    friend Container;

  public:
    T& operator*(){
        return container[id];
    }
    T* operator->(){
        return &container[id];
    }

    void free(){
        return container.free(id);
    }

  private:
    DataReference(Container& container, DataDescriptor id) noexcept:
        container(container), id(id){}

  private:
    Container& container;
    DataDescriptor id;
};