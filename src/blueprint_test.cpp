#include "Actor/Blueprint/Blueprint.hpp"

int main(void){
    Blueprint::Enemy e;

    Vector2 window_screen;

    // in actor...
    std::vector<std::pair<Predicate, Work>> behaviour;


    // make actor from blueprint
    for(auto& [when, what]: e.behaviourMap){
        switch(when){
        case Blueprint::Enemy::When::OFFSCREEN:
            behaviour.emplace_back(
                [
                    &screen=window_screen,
                    &pos=e.physicsAttribute.position.linear,
                    &size=e.physicsAttribute.volume.base
                ](void)->bool{
                    return false;
                },
                [](){}
            );
        }
    }
}