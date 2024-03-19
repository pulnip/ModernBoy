#include <algorithm>
#include "Engine/Core.hpp"
#include "Engine/Types.hpp"
#include "Engine/SystemLocator.hpp"

Core::Core(std::unique_ptr<Timer> timer) noexcept:
    timer(std::move(timer))
{
    if(this->timer==nullptr){
        logger.error("timer not valid");
    }
}

void Core::run() noexcept{
    isRunning = true;

    timer->reset();

    // 게임 세계 업데이트가 얼마나 지연되었는지,
    double lag=0.0;
    constexpr double SECOND_PER_UPDATE = 1.0 / Game::UPDATE_PER_SECOND;

    while (isRunning){
        // 최대 프레임레이트 제한용
        // timer->wait(16);

        // 최대 델타 시간 값은 0.05s
        auto deltaTime = std::min(
            0.05, timer->getDeltaTime()/1000.0
        );
        // 게임 시간은 dt만큼 지연됨.
        lag += deltaTime;

        // process input
        

        // 너무 많이 지연되면,
        while(lag >= SECOND_PER_UPDATE){
            // 게임 세계 업데이트
            __Actors().update(SECOND_PER_UPDATE);
            // 업데이트한 만큼 지연이 없어짐.
            lag -= SECOND_PER_UPDATE;
        }

        // generate output
    }
}