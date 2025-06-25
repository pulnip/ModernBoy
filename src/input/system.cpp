#include "input/system.hpp"
#include "app_state.hpp"
#include "task.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Input;

System::System(AppState& app)
:app(app){}

static InputTask parseComponent(const void* chunk);

void System::update(DeltaTime dt){
    size_t size = numInputTask();
    // calculate size only
    std::vector<InputTask> tasks(size);
    auto it = ignoranceInfo.begin();

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        // equivalent to it = ignorangeInfo.lower_bound(bit)
        while(it->first < bit) ++it;
        // has ignorance info about this bit
        if(it->first == bit){
            auto jt = it->second.cbegin();
            for(auto vit = vec.cbegin(); vit!=vec.cend(); ++vit){
                while(*jt < vit.index) ++jt;
                if(*jt == vit.index) continue;
                // Do Task
                parseComponent(*vit);
            }
        }
        else{
            // no ignorance
            for(const auto& chunk: vec){
                // Do Task
                parseComponent(chunk);
            }
        }
    }
}

static InputTask parseComponent(const void* chunk){
    InputTask task;
    TransformComponent tc;
    InputComponent ic;
    getChunk(&tc, nullptr, nullptr, &ic, chunk);
    assert(tc.actor == ic.actor);

    // app.input...
    return task;
}

size_t System::numInputTask(){
    size_t num = 0;
    // removeDangledActor();
    auto it = ignoranceInfo.begin();

    for(const auto& [bit, vec]: app.archetypeMap){
        if(!subset(bit_of<InputTask>(), bit))
            continue;
        while(it->first < bit) ++it;
        num += vec.size() - it->second.size();
        // has ignorance info about this bit
        if(it->first == bit){
            auto jt = it->second.cbegin();
            for(auto vit=vec.cbegin(); vit!=vec.cend(); ++vit){
                if(*jt <= vit.index) ++jt;
                if(vit.index < *jt) ++num;
            }
        }
    }
    return num;
}

void System::ignoreActor(EntityID actor){
    const auto& comp = app.actorTable.at(actor);
    auto it = ignoranceInfo.lower_bound(comp.bit);
    if(it->first < comp.bit){
        IgnoredIndex indices;
        indices.emplace(comp.chunkIndex);
        ignoranceInfo.emplace_hint(it, comp.bit, std::move(indices));
    }
    else{
        ignoranceInfo.at(comp.bit).insert(comp.chunkIndex);
    }
}
void System::unignoreActor(EntityID actor){
    const auto& comp = app.actorTable.at(actor);
    auto it = ignoranceInfo.lower_bound(comp.bit);
    if(it->first == comp.bit){
        ignoranceInfo.at(comp.bit).erase(comp.chunkIndex);
    }
    else{
        // warning actor already unignored.
    }
}

void System::removeDangledActor(){
    for(auto& [bit, set]: ignoranceInfo){
        std::erase_if(set,
            [bit, &table=app.actorTable]
            (const auto& index){
                for(const auto& [id, comp]: table)
                if(comp.bit==bit && comp.chunkIndex==index)
                    return false;
                return false;
            }
        );

    }
}
