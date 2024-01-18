#include <cassert>

#include "SubEngine/SubEngine.hpp"
#include "GameEngine/IGameEngine.hpp"

std::optional<std::shared_ptr<ISubEngine>>
SubEngine::query(const SubEngineName name) noexcept{
    assert(!owner.expired());
    auto ge=owner.lock();
    auto result=ge->find(name);

    if(result == nullptr){
        return std::nullopt;
    }
    return result;
}
