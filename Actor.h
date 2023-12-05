#pragma once

#include <vector>

#include "Game.h"

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
    State mState;
    // 변환
    Vector2 mPosition; // 액터의 중심점
    float mScale; // 액터의 배율(100%=1.0f)
    float mRotation; // 회전 각도(라디안)
    // 액터가 보유한 컴포넌트들
    std::vector<class Component*> mComponents;
    class Game* game;
};

class Component{
public:
    // 업데이트 순서값이 작을수록 더 빨리 갱신
    Component(class Actor* owner, int updateOrder=100);
    virtual ~Component();
    
    virtual void update(float deltaTime);
    int GetUpdateOrder() const{ return mUpdateOrder; }
protected:
    // 소유자 액터
    class Actor* mOwner;
    // 컴포넌트의 업데이트 순서
    int mUpdateOrder;
};
