#include "Components.hpp"
#include "Game.hpp"
#include "Paddle.hpp"

Paddle::Paddle(Game* game):Actor(game)
,start_position{15.0, 768/2}, fixed_velocity{0, 300}, fixed_size{15, 120}{
    tc=new TransformComponent(this,
        start_position.x, start_position.y
    );
    auto sdc=new SimpleDrawComponent(this,
        fixed_size.x, fixed_size.y,
        tc
    );
    game->drawables.emplace_back(sdc);
    auto cc=new CollisionComponent(this,
        tc, sdc
    );
    auto con_c=new ControlComponent(this,
        tc, game
    );
    AddComponent(tc);
    AddComponent(sdc);
    AddComponent(cc);
    AddComponent(con_c);
}

void Paddle::UpdateActor(float deltaTime){
    tc->velocity.y = fixed_velocity.y;
}
