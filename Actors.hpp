#pragma once

#include <concepts>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
        template<typename Concrete> requires std::derived_from<Concrete, Actor>
        static std::shared_ptr<Concrete> make(const std::weak_ptr<class Game> game) noexcept{
            struct make_shared_enabler: public Concrete{
                make_shared_enabler(const std::weak_ptr<class Game> game):Concrete(game){}
            };
            std::shared_ptr<Actor> actor=std::make_shared<make_shared_enabler>(game);
            actor->load(actor);
            game.lock()->appendActor(actor);
            return std::static_pointer_cast<Concrete>(actor);
        }
        Factory()=delete;
        ~Factory()=delete;
    };
protected:
    Actor(const std::weak_ptr<class Game> game) noexcept;
public:
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
    virtual void updateActor(const float deltaTime) noexcept{}
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

// Concrete Actor

class Paddle: public Actor{
protected:
    Paddle(const std::weak_ptr<class Game> game) noexcept;
public:
    void updateActor(const float deltaTime) noexcept override;
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class InputComponent> ic;
    std::shared_ptr<class MoveComponent> mc;
};

class Wall: public Actor{
protected:
    Wall(const std::weak_ptr<class Game> game) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class BoxComponent> bc;
    std::shared_ptr<class MoveComponent> mc;
};

class Ball: public Actor, public std::enable_shared_from_this<Ball>{
protected:
    Ball(const std::weak_ptr<class Game> game) noexcept;
public:
    void updateActor(const float deltaTime) noexcept override;
    void collideAllow(const std::weak_ptr<Actor> opponent) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class CollisionComponent> cc;
    std::shared_ptr<class MoveComponent> mc;
};

class Ship: public Actor{
public:
    void updateActor(float deltaTime) noexcept override;
protected:
    Ship(const std::weak_ptr<class Game> game) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class AnimSpriteComponent> sc;
    std::shared_ptr<class InputComponent> ic;
    std::shared_ptr<class MoveComponent> mc;
};

class Asteroid: public Actor{
protected:
    Asteroid(const std::weak_ptr<class Game> game) noexcept;
private:
    void load(const std::weak_ptr<Actor> self) noexcept override;
private:
    std::shared_ptr<class SpriteComponent> sc;
    std::shared_ptr<class MoveComponent> mc;
};
