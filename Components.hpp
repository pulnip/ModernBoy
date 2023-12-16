#pragma once

#include <memory>
#include <vector>
#include <cfloat>

// Component interface

class Component{
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
    std::vector<const std::weak_ptr<class Actor>> opponents;
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
    virtual void draw(const std::weak_ptr<class SDL_Renderer> renderer)=0;

    int getDrawOrder() const noexcept{ return drawOrder; }
protected:
    // 그리기 순서(화가 알고리즘)
    const int drawOrder;
};

// Color Box 텍스처

class BoxComponent final: public DrawComponent{
public:
    struct Color{
        char r=0, g=0, b=0, a=255;
    };

    BoxComponent(
        const std::weak_ptr<class Actor> owner,
        const int drawOrder=200
    ) noexcept :DrawComponent(owner, drawOrder){}
    ~BoxComponent()=default;

    void draw(const std::weak_ptr<class SDL_Renderer> renderer) noexcept override;
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

    virtual void draw(const std::weak_ptr<class SDL_Renderer> renderer) noexcept override;
    virtual void setTexture(const std::weak_ptr<class SDL_Texture> texture) noexcept;
private:
    std::weak_ptr<class SDL_Texture> texture;
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
    void setAnimTextures(const std::vector<const std::weak_ptr<class SDL_Texture>>& textures){
        animTextures=textures;
    }
    // 애니메이션 FPS
    float getAnimFPS() const noexcept{ return animFPS; }
    void setAnimFPS(const float fps) noexcept{ animFPS=fps; }
private:
    std::vector<const std::weak_ptr<class SDL_Texture>> animTextures;
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
    void draw(const std::weak_ptr<class SDL_Renderer> renderer) noexcept override;

    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<const std::weak_ptr<class SDL_Texture>>& textures) noexcept;
    void setScreenSize(const Vector2& size) noexcept{ screenSize=size; }
    float getScrollSpeed() const noexcept{ return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept{ scrollSpeed=speed; }

private:
    struct BGTexture{
        const std::weak_ptr<class SDL_Texture> texture;
        // 화면이 시작하는 위치
        float mOffset_x;
    };
    std::vector<BGTexture> BGTextures;
    Vector2 screenSize;
    float scrollSpeed=0.0f;
};

class MoveComponent: public Component{
public:
    MoveComponent(
        const std::weak_ptr<class Actor> owner,
        const int updateOrder=100
    ) noexcept :Component(owner, updateOrder){}
    ~MoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    float getAngularSpeed() const noexcept{ return mAngularSpeed; }
    void setAngularSpeed(const float speed) noexcept{ mAngularSpeed=speed; }
    float getForwardSpeed() const noexcept{ return mForwardSpeed; }
    void setForwardSpeed(const float speed) noexcept{ mForwardSpeed=speed; }
protected:
    // radian per second
    float mAngularSpeed;
    // unit per second
    float mForwardSpeed;
};

class InputComponent final: public MoveComponent{
public:
    InputComponent(
        std::weak_ptr<class Actor> owner,
        const int updateOrder=99
    ) noexcept :MoveComponent(owner, updateOrder){}
    ~InputComponent()=default;

    void processInput(const uint8_t* keyState) noexcept override;

    float getForwardMoveSpeed() const noexcept{ return mForwardMoveSpeed; }
    void setForwardMoveSpeed(const float speed) noexcept{ mForwardMoveSpeed=speed; }
    float getAngularMoveSpeed() const noexcept{ return mAngularMoveSpeed; }
    void setAngularMoveSpeed(const float radian) noexcept{ mAngularMoveSpeed=radian; }
    void setForwardKey(const unsigned short key) noexcept{ mForwardKey=key; }
    void setBackwardKey(const unsigned short key) noexcept{ mBackwardKey=key; }
    void setClockwiseKey(const unsigned short key) noexcept{ mClockwiseKey=key; }
    void setCounterClockwiseKey(const unsigned short key) noexcept{ mCounterClockwiseKey=key; }
private:
    float mForwardMoveSpeed;
    float mAngularMoveSpeed;

    unsigned short mForwardKey;
    unsigned short mBackwardKey;
    unsigned short mClockwiseKey;
    unsigned short mCounterClockwiseKey;
};