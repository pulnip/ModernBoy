#include "InputSystem.hpp"

using namespace std;
using namespace ModernBoy;

void InputSystem::subscribe(std::weak_ptr<InputComponent> obs){
    std::lock_guard lg(mtx);

    observers.emplace_back(std::move(obs));
}

void InputSystem::unsubscribe([[maybe_unused]] std::weak_ptr<InputComponent> obs){
    std::lock_guard lg(mtx);

    // observers.erase(
    //     std::remove(
    //         observers.begin(), observers.end(),
    //         obs
    //     ),
    //     observers.end()
    // );
}

void InputSystem::unsubscribeExpired(){
    std::lock_guard lg(mtx);

    // observers.erase(
    //     std::remove_if(
    //         observers.begin(), observers.end(),
    //         [](const std::weak_ptr<InputComponent>& wp){
    //             return wp.expired();
    //         }
    //     ),
    //     observers.end()
    // );
}
