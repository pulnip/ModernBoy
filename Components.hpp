#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "Math.hpp"

// Component interface

class Component{
public:
    struct Factory{
        template<typename T>
        static std::shared_ptr<T> make(const std::weak_ptr<class Actor> actor) noexcept{
            std::shared_ptr<Component> comp=std::make_shared<T>(actor);
            comp->load(comp);
            actor.lock()->appendComponent(comp);
            return std::static_pointer_cast<T>(comp);
        }
        Factory()=delete;
        ~Factory()=delete;
    };
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    Component(const std::weak_ptr<class Actor> owner) noexcept: owner(owner){
        assert(!owner.expired() && "owner(Actor): expired");
    }
    virtual ~Component()=default;
    
    virtual void update(const float deltaTime)=0;
    virtual void processInput(const uint8_t* keyState){}

    virtual const std::string& getName() const noexcept=0;
    int getUpdateOrder() const noexcept{ return updateOrder; }
    void setUpdateOrder(const int uo) noexcept{ updateOrder=uo; }
private:
    virtual void load(const std::weak_ptr<Component> self) noexcept{}
protected:
    // 소유자 액터
    const std::weak_ptr<class Actor> owner;
    // 컴포넌트의 업데이트 순서
    int updateOrder=0;
};

// Real Components

// 충돌 처리 컴포넌트
class CollisionComponent final: public Component{
public:
    CollisionComponent(const std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=200;
    }
    void update(const float deltaTime) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void allow(const std::weak_ptr<class Actor> opponent) noexcept{
        opponents.emplace_back(opponent);
    }
private:
    static const std::string className;
    std::vector<std::weak_ptr<class Actor>> opponents;
};

// 2D Graphics interface
class DrawComponent: public Component{
public:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(const std::weak_ptr<class Actor> owner) noexcept;
    virtual ~DrawComponent()=default;

    virtual void update(const float deltaTime) noexcept override{}
    virtual void draw(class SDL_Renderer* renderer)=0;

    virtual const std::string& getName() const noexcept override{ return className; }
    int getDrawOrder() const noexcept{ return drawOrder; }
private:
    void load(const std::weak_ptr<Component> self) noexcept override;
protected:
    // 그리기 순서(화가 알고리즘)
    int drawOrder=0;
private:
    static const std::string className;
};

// Color Box 텍스처
class BoxComponent final: public DrawComponent{
public:
    struct Color{
        using byte=uint8_t;
        byte r=0, g=0, b=0, a=255;
    };
public:
    BoxComponent(const std::weak_ptr<class Actor> owner) noexcept: DrawComponent(owner){
        drawOrder=200;
    }
    ~BoxComponent()=default;

    void draw(class SDL_Renderer* renderer) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void setTexture(const Color& color, const Vector2& size) noexcept;
private:
    static const std::string className;
    Color color;
};

// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent{
public:
    SpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: DrawComponent(owner){
        drawOrder=201;
    }
    virtual ~SpriteComponent()=default;

    virtual void draw(class SDL_Renderer* renderer) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
    virtual void setTexture(class SDL_Texture* texture) noexcept;
private:
    static const std::string className;
    class SDL_Texture* texture;
};

// 애니메이션 텍스처
class AnimSpriteComponent final: public SpriteComponent{
public:
    AnimSpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: SpriteComponent(owner){
        drawOrder=202;
    }
    ~AnimSpriteComponent()=default;

    // 애니메이션을 프레임마다 갱신
    void update(const float deltaTime) noexcept override;
    
    const std::string& getName() const noexcept override{ return className; }
    // 애니메이션에 사용되는 텍스처 설정
    void setAnimTextures(const std::vector<class SDL_Texture*>& textures){
        animTextures=textures;
    }
    // 애니메이션 FPS
    float getAnimFPS() const noexcept{ return animFPS; }
    void setAnimFPS(const float fps) noexcept{ animFPS=fps; }
private:
    static const std::string className;
    std::vector<class SDL_Texture*> animTextures;
    // 현재 프레임
    float currFrame=0.0f;
    float animFPS=8.0f;
};

// 스크롤되는 배경
class BGSpriteComponent final: public SpriteComponent{
public:
    BGSpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: SpriteComponent(owner){
        drawOrder=100;
    }
    ~BGSpriteComponent()=default;
    
    void update(const float deltaTime) noexcept override;
    void draw(class SDL_Renderer* renderer) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<class SDL_Texture*>& textures) noexcept;
    void setScreenSize(const Vector2& size) noexcept{ screenSize=size; }
    float getScrollSpeed() const noexcept{ return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept{ scrollSpeed=speed; }
private:
    static const std::string className;
    struct BGTexture{
        class SDL_Texture* texture;
        // 화면이 시작하는 위치
        float offset_x;
    };
    std::vector<BGTexture> BGTextures;
    Vector2 screenSize;
    float scrollSpeed=0.0f;
};

class MoveComponent: public Component{
public:
    MoveComponent(const std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=100;
    }
    ~MoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
public:
    // unit per second
    class Velocity{
    public:
        Vector2& operator()() noexcept{ return velocity; }
        Vector2& operator()(Math::Real rotation, float forwardSpeed){
            return velocity = Vector2::forward(rotation) * forwardSpeed;
        }
    private:
        Vector2 velocity;
    } velocity;
private:
    static const std::string className;
};
class InputComponent final: public MoveComponent{
public:
    InputComponent(std::weak_ptr<class Actor> owner) noexcept: MoveComponent(owner){
        updateOrder=99;
    }
    ~InputComponent()=default;

    void processInput(const uint8_t* keyState) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void setXPKey(const uint8_t key) noexcept{ xPositiveKey=key; }
    void setXNKey(const uint8_t key) noexcept{ xNegativeKey=key; }
    void setYPKey(const uint8_t key) noexcept{ yPositiveKey=key; }
    void setYNKey(const uint8_t key) noexcept{ yNegativeKey=key; }
private:
    static const std::string className;
    uint8_t xPositiveKey;
    uint8_t xNegativeKey;
    uint8_t yPositiveKey;
    uint8_t yNegativeKey;
};


class AngularMoveComponent: public MoveComponent{
public:
    AngularMoveComponent(const std::weak_ptr<class Actor> owner) noexcept: MoveComponent(owner){
        updateOrder=100;
    }
    ~AngularMoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
    void setForwardSpeed(const float speed) noexcept{ forwardSpeed=speed; }
    void setAngularSpeed(const Math::Radian speed) noexcept{ angularSpeed=speed; }
private:
    static const std::string className;
    // unit per second
    float forwardSpeed;
    // radian per second
    Math::Radian angularSpeed;
};
class InputComponentP final: public AngularMoveComponent{
public:
    InputComponentP(std::weak_ptr<class Actor> owner) noexcept: AngularMoveComponent(owner){
        updateOrder=99;
    }
    ~InputComponentP()=default;

    void processInput(const uint8_t* keyState) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void setForwardSpeedPreset(const float speed) noexcept{ forwardSpeedPreset=speed; }
    void setAngularSpeedPreset(const Math::Radian speed) noexcept{ angularSpeedPreset=speed; }
    void setForwardKey(const uint8_t key) noexcept{ forwardKey=key; }
    void setBackwardKey(const uint8_t key) noexcept{ backwardKey=key; }
    void setClockwiseKey(const uint8_t key) noexcept{ clockwiseKey=key; }
    void setCounterClockwiseKey(const uint8_t key) noexcept{ counterClockwiseKey=key; }
private:
    static const std::string className;
    float forwardSpeedPreset;
    Math::Radian angularSpeedPreset;

    uint8_t forwardKey;
    uint8_t backwardKey;
    uint8_t clockwiseKey;
    uint8_t counterClockwiseKey;
};