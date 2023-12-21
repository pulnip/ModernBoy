#pragma once

#include <memory>
#include <vector>

#include "Math.hpp"

// Component interface

class Component: private std::enable_shared_from_this<Component>{
    friend class Actor;
public:
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    Component(
        const std::weak_ptr<class Actor> owner,
        const int updateOrder
    ) noexcept;
    virtual ~Component()=default;
    
    virtual void update(const float deltaTime)=0;
    virtual void processInput(const uint8_t* keyState){}

    int getUpdateOrder() const noexcept{ return updateOrder; }
protected:
    // this 대체용
    std::weak_ptr<Component> self;
    // 소유자 액터
    const std::weak_ptr<class Actor> owner;
    // 컴포넌트의 업데이트 순서
    const int updateOrder;

};

// Real Components

// 충돌 처리 컴포넌트
class CollisionComponent final: public Component{
public:
    CollisionComponent(
        const std::weak_ptr<class Actor> owner,
        const int updateOrder=200
    ) noexcept :Component(owner, updateOrder){}
    ~CollisionComponent()=default;

    void update(const float deltaTime) noexcept override;

    void allow(const std::weak_ptr<class Actor> opponent) noexcept;
    void disallow(const std::weak_ptr<class Actor> opponent) noexcept;
private:
    std::vector<std::weak_ptr<class Actor>> opponents;
};

// 2D Graphics interface
class DrawComponent: public Component{
public:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder
    ) noexcept;
    virtual ~DrawComponent();

    virtual void update(const float deltaTime) noexcept override{}
    virtual void draw(class SDL_Renderer* renderer)=0;

    int getDrawOrder() const noexcept{ return drawOrder; }
protected:
    // 그리기 순서(화가 알고리즘)
    const int drawOrder;
};

// Color Box 텍스처

class BoxComponent final: public DrawComponent{
public:
    struct Color{
        using byte=uint8_t;
        byte r=0, g=0, b=0, a=255;
    };

    BoxComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder=200
    ) noexcept :DrawComponent(owner, drawOrder){}
    ~BoxComponent()=default;

    void draw(class SDL_Renderer* renderer) noexcept override;
    void setTexture(const Color& color, const Vector2& size) noexcept;
private:
    Color color;
};

// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent{
public:
    SpriteComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder=201
    )noexcept :DrawComponent(owner, drawOrder){}
    virtual ~SpriteComponent()=default;

    virtual void draw(class SDL_Renderer* renderer) noexcept override;
    virtual void setTexture(class SDL_Texture* texture) noexcept;
private:
    class SDL_Texture* texture;
};

// 애니메이션 텍스처
class AnimSpriteComponent final: public SpriteComponent{
public:
    AnimSpriteComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder=202
    ) noexcept :SpriteComponent(owner, drawOrder){}
    ~AnimSpriteComponent()=default;

    // 애니메이션을 프레임마다 갱신
    void update(const float deltaTime) noexcept override;
    // 애니메이션에 사용되는 텍스처 설정
    void setAnimTextures(const std::vector<class SDL_Texture*>& textures){
        animTextures=textures;
    }
    // 애니메이션 FPS
    float getAnimFPS() const noexcept{ return animFPS; }
    void setAnimFPS(const float fps) noexcept{ animFPS=fps; }
private:
    std::vector<class SDL_Texture*> animTextures;
    // 현재 프레임
    float currFrame=0.0f;
    float animFPS=8.0f;
};

// 스크롤되는 배경
class BGSpriteComponent final: public SpriteComponent{
public:
    BGSpriteComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder=100
    ) noexcept :SpriteComponent(owner, drawOrder){}
    ~BGSpriteComponent()=default;
    
    void update(const float deltaTime) noexcept override;
    void draw(class SDL_Renderer* renderer) noexcept override;

    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<class SDL_Texture*>& textures) noexcept;
    void setScreenSize(const Vector2& size) noexcept{ screenSize=size; }
    float getScrollSpeed() const noexcept{ return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept{ scrollSpeed=speed; }

private:
    struct BGTexture{
        class SDL_Texture* texture;
        // 화면이 시작하는 위치
        float offset_x;
    };
    std::vector<BGTexture> BGTextures;
    Vector2 screenSize;
    float scrollSpeed=0.0f;
};

class AngularMoveComponent: public Component{
public:
    AngularMoveComponent(
        const std::weak_ptr<class Actor> owner,
        const int updateOrder=100
    ) noexcept :Component(owner, updateOrder){}
    ~AngularMoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    float getAngularSpeed() const noexcept{ return angularSpeed; }
    void setAngularSpeed(const float speed) noexcept{ angularSpeed=speed; }
    float getForwardSpeed() const noexcept{ return forwardSpeed; }
    void setForwardSpeed(const float speed) noexcept{ forwardSpeed=speed; }
protected:
    // radian per second
    float angularSpeed;
    // unit per second
    float forwardSpeed;
};
class InputComponentP final: public AngularMoveComponent{
public:
    InputComponentP(
        std::weak_ptr<class Actor> owner,
        const int updateOrder=99
    ) noexcept :AngularMoveComponent(owner, updateOrder){}
    ~InputComponentP()=default;

    void processInput(const uint8_t* keyState) noexcept override;

    float getForwardMoveSpeed() const noexcept{ return forwardMoveSpeed; }
    void setForwardMoveSpeed(const float speed) noexcept{ forwardMoveSpeed=speed; }
    float getAngularMoveSpeed() const noexcept{ return angularMoveSpeed; }
    void setAngularMoveSpeed(const float radian) noexcept{ angularMoveSpeed=radian; }
    void setForwardKey(const uint8_t key) noexcept{ forwardKey=key; }
    void setBackwardKey(const uint8_t key) noexcept{ backwardKey=key; }
    void setClockwiseKey(const uint8_t key) noexcept{ clockwiseKey=key; }
    void setCounterClockwiseKey(const uint8_t key) noexcept{ counterClockwiseKey=key; }
private:
    float forwardMoveSpeed;
    float angularMoveSpeed;

    uint8_t forwardKey;
    uint8_t backwardKey;
    uint8_t clockwiseKey;
    uint8_t counterClockwiseKey;
};

class AbsoluteMoveComponent: public Component{
public:
    AbsoluteMoveComponent(
        const std::weak_ptr<class Actor> owner,
        const int updateOrder=100
    ) noexcept :Component(owner, updateOrder){}
    ~AbsoluteMoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    const Vector2& getMoveVelocity() const noexcept{ return moveVelocity; }
    void setMoveVelocity(const Vector2& v){ moveVelocity=v; }
protected:
    // unit per second
    Vector2 moveVelocity;
};
class InputComponentA final: public AbsoluteMoveComponent{
public:
    InputComponentA(
        std::weak_ptr<class Actor> owner,
        const int updateOrder=99
    ) noexcept :AbsoluteMoveComponent(owner, updateOrder){}
    ~InputComponentA()=default;

    void processInput(const uint8_t* keyState) noexcept override;

    void setXPKey(const uint8_t key) noexcept{ xPositiveKey=key; }
    void setXNKey(const uint8_t key) noexcept{ xNegativeKey=key; }
    void setYPKey(const uint8_t key) noexcept{ yPositiveKey=key; }
    void setYNKey(const uint8_t key) noexcept{ yNegativeKey=key; }
private:
    uint8_t xPositiveKey;
    uint8_t xNegativeKey;
    uint8_t yPositiveKey;
    uint8_t yNegativeKey;
};