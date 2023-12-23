#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "Math.hpp"

// Component interface

class Component{
public:
    struct Factory{
        template<typename Concrete> requires std::derived_from<Concrete, Component>
        static std::shared_ptr<Concrete> make(const std::weak_ptr<class Actor> actor) noexcept{
            struct make_shared_enabler: public Concrete{
                make_shared_enabler(const std::weak_ptr<class Actor> actor):Concrete(actor){}
            };
            std::shared_ptr<Component> comp=std::make_shared<make_shared_enabler>(actor);
            comp->load(comp);
            actor.lock()->appendComponent(comp);
            return std::static_pointer_cast<Concrete>(comp);
        }
        Factory()=delete;
        ~Factory()=delete;
    };
public:
    virtual ~Component()=default;
    
    virtual void update(const float deltaTime) noexcept=0;
    virtual void processInput(const uint8_t* keyState){}

    virtual const std::string& getName() const noexcept=0;
    int getUpdateOrder() const noexcept{ return updateOrder; }
    void setUpdateOrder(const int uo) noexcept{ updateOrder=uo; }
protected:
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    Component(const std::weak_ptr<class Actor> owner) noexcept;
private:
    virtual void load(const std::weak_ptr<Component> self) noexcept{}
protected:
    // 소유자 액터
    const std::weak_ptr<class Actor> owner;
    // 컴포넌트의 업데이트 순서
    int updateOrder=0;
};

// Concrete Component

// 충돌 처리 컴포넌트
class CollisionComponent: public Component{
public:
    void update(const float deltaTime) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void allow(const std::weak_ptr<class Actor> opponent) noexcept{
        opponents.emplace_back(opponent);
    }
protected:
    CollisionComponent(const std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=200;
    }
public:
    static const std::string className;
private:
    std::vector<std::weak_ptr<class Actor>> opponents;
};

// 2D Graphics interface
class DrawComponent: public Component{
public:
    virtual ~DrawComponent()=default;

    virtual void update(const float deltaTime) noexcept override{}
    virtual void draw(class SDL_Renderer* renderer)=0;

    virtual const std::string& getName() const noexcept override{ return className; }
    int getDrawOrder() const noexcept{ return drawOrder; }
protected:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(const std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=300;
    }
private:
    void load(const std::weak_ptr<Component> self) noexcept override;
public:
    static const std::string className;
protected:
    // 그리기 순서(화가 알고리즘)
    int drawOrder=0;
};
// Color Box 텍스처
class BoxComponent: public DrawComponent{
public:
    struct Color{
        using byte=uint8_t;
        byte r=0, g=0, b=0, a=255;
    };
public:
    ~BoxComponent()=default;

    void draw(class SDL_Renderer* renderer) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    void setTexture(const Color& color, const Vector2& size) noexcept;
protected:
    BoxComponent(const std::weak_ptr<class Actor> owner) noexcept: DrawComponent(owner){
        drawOrder=200;
    }
public:
    static const std::string className;
private:
    Color color;
};
// 단일 스프라이트 텍스처
class SpriteComponent: public DrawComponent{
public:
    virtual ~SpriteComponent()=default;

    virtual void draw(class SDL_Renderer* renderer) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
    virtual void setTexture(class SDL_Texture* texture) noexcept;
protected:
    SpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: DrawComponent(owner){
        drawOrder=201;
    }
public:
    static const std::string className;
private:
    class SDL_Texture* texture;
};
// 애니메이션 텍스처
class AnimSpriteComponent: public SpriteComponent{
public:
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
protected:
    AnimSpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: SpriteComponent(owner){
        drawOrder=202;
    }
public:
    static const std::string className;
private:
    std::vector<class SDL_Texture*> animTextures;
    // 현재 프레임
    float currFrame=0.0f;
    float animFPS=8.0f;
};
// 스크롤되는 배경
class BGSpriteComponent: public SpriteComponent{
public:
    ~BGSpriteComponent()=default;
    
    void update(const float deltaTime) noexcept override;
    void draw(class SDL_Renderer* renderer) noexcept override;

    const std::string& getName() const noexcept override{ return className; }
    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<class SDL_Texture*>& textures) noexcept;
    void setScreenSize(const Vector2& size) noexcept{ screenSize=size; }
    float getScrollSpeed() const noexcept{ return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept{ scrollSpeed=speed; }
protected:
    BGSpriteComponent(const std::weak_ptr<class Actor> owner) noexcept: SpriteComponent(owner){
        drawOrder=100;
    }
public:
    static const std::string className;
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

class MoveComponent: public Component{
public:
    ~MoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
protected:
    MoveComponent(const std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=201;
    }
public:
    // unit per second
    class Velocity{
    public:
        Vector2& operator()() noexcept{ return velocity; }
        Vector2& operator()(Math::Real rotation, float forwardSpeed) noexcept{
            return velocity = Vector2::forward(rotation) * forwardSpeed;
        }
    private:
        Vector2 velocity;
    } velocity;
public:
    static const std::string className;
};
class AngularMoveComponent: public MoveComponent{
public:
    ~AngularMoveComponent()=default;

    void update(const float deltaTime) noexcept override;

    virtual const std::string& getName() const noexcept override{ return className; }
    void setForwardSpeed(const float speed) noexcept{ forwardSpeed=speed; }
    void setAngularSpeed(const Math::Radian speed) noexcept{ angularSpeed=speed; }
protected:
    AngularMoveComponent(const std::weak_ptr<class Actor> owner) noexcept: MoveComponent(owner){}
public:
    static const std::string className;
private:
    // unit per second
    float forwardSpeed;
    // radian per second
    Math::Radian angularSpeed;
};

#warning "make key map"
class InputComponent: public Component{
public:
    ~InputComponent()=default;

    void processInput(const uint8_t* keyState) noexcept override;
    virtual void update(const float deltaTime) noexcept override{}

    const std::string& getName() const noexcept override{ return className; }
    void setKey(const uint8_t key, std::function<void(void)> behavior);
    void setSpeedPreset(const Vector2& v) noexcept{ speedPreset=v; }
    void setXPKey(const uint8_t key) noexcept{ xPositiveKey=key; }
    void setXNKey(const uint8_t key) noexcept{ xNegativeKey=key; }
    void setYPKey(const uint8_t key) noexcept{ yPositiveKey=key; }
    void setYNKey(const uint8_t key) noexcept{ yNegativeKey=key; }
protected:
    InputComponent(std::weak_ptr<class Actor> owner) noexcept: Component(owner){
        updateOrder=100;
    }
public:
    static const std::string className;
protected:
    std::map<uint8_t, std::function<void(void)>> keymap;
private:
    Vector2 speedPreset;

    uint8_t xPositiveKey;
    uint8_t xNegativeKey;
    uint8_t yPositiveKey;
    uint8_t yNegativeKey;
};
class AngularInputComponent: public InputComponent{
public:
    ~AngularInputComponent()=default;

    void processInput(const uint8_t* keyState) noexcept override;
    void update(const float deltaTime) noexcept override{}

    const std::string& getName() const noexcept override{ return className; }
    void setForwardSpeedPreset(const float speed) noexcept{ forwardSpeedPreset=speed; }
    void setAngularSpeedPreset(const Math::Radian speed) noexcept{ angularSpeedPreset=speed; }
    void setForwardKey(const uint8_t key) noexcept{ forwardKey=key; }
    void setBackwardKey(const uint8_t key) noexcept{ backwardKey=key; }
    void setClockwiseKey(const uint8_t key) noexcept{ clockwiseKey=key; }
    void setCounterClockwiseKey(const uint8_t key) noexcept{ counterClockwiseKey=key; }
protected:
    AngularInputComponent(std::weak_ptr<class Actor> owner) noexcept: InputComponent(owner){}
public:
    static const std::string className;
private:
    float forwardSpeedPreset;
    Math::Radian angularSpeedPreset;

    uint8_t forwardKey;
    uint8_t backwardKey;
    uint8_t clockwiseKey;
    uint8_t counterClockwiseKey;
};