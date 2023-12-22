#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Game.hpp"
#include "Math.hpp"

// Actor interface

class Actor{
public:
    // 액터의 상태를 추적하는 데 사용
    enum State{
        EActive, EPaused, EDead
    };
public:
    struct Factory{
        template<typename T>
        static std::shared_ptr<T> make(const std::weak_ptr<class Game> game) noexcept{
            std::shared_ptr<Actor> actor=std::make_shared<T>(game);
            actor->load(actor);
            game.lock()->appendActor(actor);
            return std::static_pointer_cast<T>(actor);
        }
        Factory()=delete;
        ~Factory()=delete;
    };
    Actor(const std::weak_ptr<class Game> game) noexcept;
    virtual ~Actor()=default;

    // Game에서 input을 받는 함수
    void processInput(const uint8_t* keyState) noexcept;
    // 특정 액터를 위한 입력 코드
    virtual void processActorInput(const uint8_t* keyState){}
    // Game에서 호출하는 함수
    void update(const float deltaTime) noexcept;
private:
    void updateComponents(const float deltaTime) noexcept;
public:
    // 특정 액터에 특화된 업데이트 코드
    virtual void updateActor(float deltaTime){}
public:
    // Getters/Setters
    const State& getState() const noexcept{ return state; }
    const std::weak_ptr<class Game>& getGame() const noexcept{ return game; }

    Vector2 getSize() const noexcept{ return scale*baseSize; }

    void appendComponent(const std::shared_ptr<class Component> component) noexcept;
    std::weak_ptr<class Component> queryComponent(const std::string& name) noexcept;
private:
    void orderComponents() noexcept;
    virtual void load(const std::weak_ptr<Actor> self) noexcept{}
public:
    Vector2 position;
    Vector2 baseSize;
    Math::Real scale=1.0;
    Math::Radian rotation=0.0;
protected:
    // 액터 구현체가 보유한 컴포넌트들
    std::vector<std::shared_ptr<class Component>> components;
    // queryComponent에서 사용
    std::map<std::string, std::shared_ptr<class Component>> componentMap;

    State state=EActive;
    const std::weak_ptr<class Game> game;
private:
    bool isOrdered=true;
};

// Real Actors

class Paddle final: public Actor{
public:
    Paddle(const std::weak_ptr<class Game> game) noexcept;
    void updateActor(float deltaTime) noexcept override{}
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class InputComponent> ic;
};

class Wall final: public Actor{
public:
    Wall(const std::weak_ptr<class Game> game) noexcept;
    void updateActor(float deltaTime) override{}
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class BoxComponent> bc;
};

class Ball final: public Actor, public std::enable_shared_from_this<Ball>{
public:
    Ball(const std::weak_ptr<class Game> game) noexcept;
    void updateActor(const float deltaTime) noexcept override;
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class MoveComponent> mc;
};

class Ship final: public Actor{
public:
    Ship(const std::weak_ptr<class Game> game) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class InputComponentP> ic;
};

class Asteroid final: public Actor{
public:
    Asteroid(const std::weak_ptr<class Game> game) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class SpriteComponent> sc;
    std::shared_ptr<class AngularMoveComponent> mc;
};
