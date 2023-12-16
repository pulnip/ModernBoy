#pragma once

#include <memory>
#include <vector>

#include "Math.hpp"

// Actor interface

class Actor{
public:
    // 액터의 상태를 추적하는 데 사용
    enum State{
        EActive, EPaused, EDead
    };

    Actor(const std::weak_ptr<class Game> game) noexcept;
    virtual ~Actor()=default;

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

    void appendComponent(const std::shared_ptr<class Component>& component) noexcept;
    std::weak_ptr<class Component> queryComponent(const class Component* component) const noexcept;
    void removeComponent(const class Component* component) noexcept;
private:
    void orderComponents() noexcept;

protected:
    State state=EActive;
    const std::weak_ptr<class Game> game;
    
    Vector2 position;
    Vector2 velocity;
    Vector2 baseSize;
    Math::Real scale=1.0;
    Math::Radian rotation=0.0;
private:
    // 액터가 보유한 컴포넌트들
    std::vector<const std::shared_ptr<class Component>> components;
    decltype(components)::const_iterator find(const class Component* component) const noexcept;
    bool isOrdered=true;
};

// Real Actors

class Paddle final: public Actor{
public:
    Paddle(class Game* game);
    ~Paddle()=default;

    void updateActorFirst(float deltaTime) override;
    void updateActor(float deltaTime) override;
    void collideAllow(Actor* opponent);
private:
    const float fixed_velocity_y;
    class CollisionComponent* cc;
};

class Wall final: public Actor{
public:
    Wall(class Game* game, int x, int y, int w, int h);
    ~Wall()=default;

    void updateActor(float deltaTime) override{}
};

class Ball final: public Actor{
public:
    Ball(class Game* game, int x, int y, int w, int h);
    ~Ball()=default;

    void updateActor(float deltaTime) override;
    void collideAllow(Actor* opponent);
private:
    class CollisionComponent* cc;
};

class Ship final: public Actor{
public:
    Ship(class Game* game);
};

class Asteroid final: public Actor{
public:
    Asteroid(class Game* game);
};
