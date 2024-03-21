#pragma once

template<typename T, unsigned int MAX>
class Localized{
    using ID = unsigned int;

  public:
    Localized(){
        static_assert(MAX <= 256 * sizeof(ID));
        for(ID id=0; id < MAX; ++id){
            queue[id] = id;
        }
    }

    ID allocate(bool init){
        if((end+1)%MAX == start){
            return -1;
        }


    }

    void free(ID id){

    }

  private:
    T memory[MAX];
    ID queue[MAX];
    unsigned int start=0, end=0;
};