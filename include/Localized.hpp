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
using ID_t=unsigned int;

template<Initializable T, ID_t MAX>
    requires(MAX < 256*sizeof(ID_t))
class Localized{
  public:
    Localized(){
        for(ID_t i=0; i < MAX; ++i){
            stack[i] = i;
        }
        assert(isUnique());
    }

    bool isEmpty() const noexcept{ return head==0; }
    bool isFull() const noexcept{ return head==MAX; }

    std::optional<ID_t> allocate(bool init=false){
        assert(isUnique());
        // all memory is allocated.
        if(isFull()){
            // so, cannot return id.
            return std::nullopt;
        }

        // pop id from non-assigned section list.
        ID_t result=stack[head++];
        // cannot assign already-assigned section.
        assert(not memory[result].assignFlag);

        memory[result].assignFlag = true;
        // if request init T
        if(init){
            memory[result].data.init();
        }

        return result;
        // memory[poped-id] is now no longer assignable.
    }

    void free(ID_t id){
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
    bool isUnique(){
        // allocated memory id must be unique
        bool id_exist[MAX]={false, };
        for(auto i=head; i<MAX; ++i){
            auto& exist=id_exist[stack[i]];
            if(exist) return false;
            exist=true;
        }

        return true;
    }

  private:
    struct Section{
        bool assignFlag=false;
        T data;
    };
    Section memory[MAX];

    // non-assigned section list
    ID_t stack[MAX];
    // stack[MAX-1] is bottom
    ID_t head=0;
};