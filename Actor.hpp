#pragma once

#include <vector>

#include "Vector2.hpp"

class Actor{
public:
    // 액터의 상태를 추적하는 데 사용
    enum State{
        EActive, EPaused, EDead
    };

    Actor(class Game* game);
    virtual ~Actor();

    // Game에서 호출하는 Update함수
    void Update(float deltaTime);
    // 액터에 부착된 모든 컴포넌트를 업데이트
    void UpdateComponents(float deltaTime);
    // 특정 액터에 특화된 업데이트 코드
    virtual void UpdateActor(float deltaTime);

    // Getters/Setters
    State GetState() const{ return mState; }

    void AddComponent(class Component* component);
    void RemoveComponent(class Component* component);
private:
    State mState=EActive;
    // 액터가 보유한 컴포넌트들
    std::vector<class Component*> mComponents;
    
    class Game* game;
};
