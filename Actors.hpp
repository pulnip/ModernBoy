#pragma once

#include <memory>
#include <vector>

#include "Math.hpp"

// Actor interface

class Actor: private std::enable_shared_from_this<Actor>{
public:
    // 액터의 상태를 추적하는 데 사용
    enum State{
        EActive, EPaused, EDead
    };
public:
    Actor(const std::weak_ptr<class Game> game) noexcept;
    virtual ~Actor(){}

    // Game에서 호출하는 함수
    void processInput(const uint8_t* keyState) noexcept;
    void update(const float deltaTime) noexcept;
    void updateComponents(const float deltaTime) noexcept;
    // 특정 액터를 위한 입력 코드
    virtual void processActorInput(const uint8_t* keyState){}
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActor(float deltaTime){}

    // Getters/Setters
    const State& getState() const noexcept{ return state; }
    const std::weak_ptr<class Game>& getGame() const noexcept{ return game; }

    const Vector2& getPosition() const noexcept{ return position; }
    void setPosition(const Vector2& other) noexcept{ position = other; }
    const Vector2& getVelocity() const noexcept{ return velocity; }
    void setVelocity(const Vector2& other) noexcept{ velocity = other; }
    const Vector2& getBaseSize() const noexcept{ return baseSize; }
    void setBaseSize(const Vector2& other) noexcept{ baseSize=other; }
    const Math::Real& getScale() const noexcept{ return scale; }
    void setScale(const Math::Real& other) noexcept{ scale=other; }
    Vector2 getSize() const noexcept{ return scale*baseSize; }
    const Math::Real& getRotation() const noexcept{ return rotation; }
    void setRotation(const Math::Real& other) noexcept{ rotation=other; }

    void appendComponent(const std::shared_ptr<class Component> component) noexcept;

    void orderComponents() noexcept;

protected:
    std::weak_ptr<Actor> self;
    // 액터 구현체가 보유한 컴포넌트들
    std::vector<std::weak_ptr<class Component>> components;

    State state=EActive;
    const std::weak_ptr<class Game> game;
    
    Vector2 position;
    Vector2 velocity;
    Vector2 baseSize;
    Math::Real scale=1.0;
    Math::Radian rotation=0.0;
private:
    bool isOrdered=true;
};

// Real Actors

class Paddle final: public Actor{
public:
    Paddle(const std::weak_ptr<class Game> game) noexcept;
    ~Paddle()=default;

    void updateActor(float deltaTime) noexcept override;
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class AbsoluteMoveComponent> mc;
};

class Wall final: public Actor{
public:
    Wall( const std::weak_ptr<class Game> game,
        const Vector2& pos,
        const Vector2& size
    ) noexcept;
    ~Wall()=default;

    void updateActor(float deltaTime) override{}
private:
    std::shared_ptr<class BoxComponent> bc;
};

class Ball final: public Actor{
public:
    Ball(const std::weak_ptr<class Game> game,
        const Vector2& pos,
        const Vector2& size
    ) noexcept;
    ~Ball()=default;

    void updateActor(const float deltaTime) noexcept override;
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class CollisionComponent> cc;
};

class Ship final: public Actor{
public:
    Ship(const std::weak_ptr<class Game> game) noexcept;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class InputComponentP> ic;
};

class Asteroid final: public Actor{
public:
    Asteroid(const std::weak_ptr<class Game> game) noexcept;
private:
    std::shared_ptr<class SpriteComponent> sc;
    std::shared_ptr<class AngularMoveComponent> mc;
};
