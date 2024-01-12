#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "Makable.hpp"
#include "Math.hpp"
#include "Observable.hpp"
#include "PubSubMessage.hpp"
#include "Skin.hpp"

class Actor;

enum class ComponentName {
    Component,

    CollisionComponent,

    DrawComponent,
    BoxComponent,
    SpriteComponent,
    AnimSpriteComponent,
    BGSpriteComponent,

    MoveComponent,

    InputComponent,

    AIComponent
};

// Component interface

class Component : public Makable<Component, Actor>, public Observable<PSMSG::Lifetime, Component> {
  public:
    virtual ~Component() = default;

    virtual void processInput(const uint8_t *keyState) {}
    virtual void update(const float &deltaTime) noexcept = 0;

    virtual ComponentName getName() const noexcept = 0;
    int getUpdateOrder() const noexcept { return updateOrder; }

  protected:
    Component(const std::weak_ptr<Actor> owner) noexcept;
    virtual void postConstruct() noexcept override;

  protected:
    const std::weak_ptr<Actor> owner;
    // 컴포넌트의 업데이트 순서
    // updateOrder이 작을수록 더 빨리 갱신
    // input 계열: 100 to 199
    // 연산 계열: 200 to 299
    // output계열: 300 to 399
    int updateOrder = 0;
};

// Concrete Component

// CollisionComponent

// 충돌 처리 컴포넌트
class CollisionComponent : public Component {
  public:
    void update(const float &deltaTime) noexcept override;

    void allow(const std::weak_ptr<Actor> opponent) noexcept {
        opponents.emplace_back(opponent);
    }

    ComponentName getName() const noexcept override {
        return ComponentName::CollisionComponent;
    }

  protected:
    CollisionComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 200;
    }

  private:
    std::vector<std::weak_ptr<class Actor>> opponents;
};

// Draw Component

// 2D Graphics interface
class DrawComponent : public Component {
  public:
    virtual void update(const float &deltaTime) noexcept override {}
    virtual void draw() = 0;

    virtual ComponentName getName() const noexcept override {
        return ComponentName::DrawComponent;
    }
    int getDrawOrder() const noexcept { return drawOrder; }

  protected:
    // drawOrder이 작을수록 더 뒤에 위치
    // 배경 계열: 100 to 199
    // 일반 오브젝트 계열: 200 to 299
    // player계열: 300 to 399
    DrawComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 300;
    }

  private:
    void postConstruct() noexcept override;

  protected:
    // 그리기 순서(화가 알고리즘)
    int drawOrder = 0;
};

// Box Component

class TrueColor;
class ColorRect;

// Color Box 텍스처
class BoxComponent : public DrawComponent, public Observable<ColorRect> {
  public:
    void draw() noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::BoxComponent;
    }
    void setTexture(const TrueColor &color, const Vector2 &size) noexcept;

  protected:
    BoxComponent(const std::weak_ptr<Actor> owner) noexcept
        : DrawComponent(owner) {
        drawOrder = 200;
    }

    TrueColor color;
};

// Sprite Component

class SDL_Texture;
class SDL_Sprite;

// 단일 스프라이트 텍스처
class SpriteComponent : public DrawComponent, public Observable<SDL_Sprite> {
  public:
    virtual void draw() noexcept override;

    virtual ComponentName getName() const noexcept override {
        return ComponentName::SpriteComponent;
    }
    virtual void setTexture(SDL_Texture *texture) noexcept;

  protected:
    SpriteComponent(const std::weak_ptr<Actor> owner) noexcept
        : DrawComponent(owner) {
        drawOrder = 201;
    }

    SDL_Texture *texture;
};

// Animation Sprite Component

// 애니메이션 텍스처
class AnimSpriteComponent : public SpriteComponent {
  public:
    // 애니메이션을 프레임마다 갱신
    void update(const float &deltaTime) noexcept override;

    virtual ComponentName getName() const noexcept override {
        return ComponentName::AnimSpriteComponent;
    }
    // 애니메이션에 사용되는 텍스처 설정
    void setAnimTextures(const std::vector<class SDL_Texture *> &textures) {
        animTextures = textures;
    }
    // 애니메이션 FPS
    float getAnimFPS() const noexcept { return animFPS; }
    void setAnimFPS(const float fps) noexcept { animFPS = fps; }

  protected:
    AnimSpriteComponent(const std::weak_ptr<Actor> owner) noexcept
        : SpriteComponent(owner) {
        drawOrder = 202;
    }

    std::vector<class SDL_Texture *> animTextures;
    // 현재 프레임
    float currFrame = 0.0f;
    float animFPS = 8.0f;
};

// Background Sprite Component

// 스크롤되는 배경
class BGSpriteComponent : public SpriteComponent {
  public:
    void update(const float &deltaTime) noexcept override;
    void draw() noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::BGSpriteComponent;
    }
    // 배경용 텍스처 설정
    void setBGTextures(const std::vector<class SDL_Texture *> &textures) noexcept;
    void setScreenSize(const Vector2 &size) noexcept { screenSize = size; }
    float getScrollSpeed() const noexcept { return scrollSpeed; }
    void setScrollSpeed(const float speed) noexcept { scrollSpeed = speed; }

  protected:
    BGSpriteComponent(const std::weak_ptr<Actor> owner) noexcept
        : SpriteComponent(owner) {
        drawOrder = 100;
    }

    struct BGTexture {
        class SDL_Texture *texture;
        // 화면이 시작하는 위치
        float offset_x;
    };
    std::vector<BGTexture> BGTextures;
    Vector2 screenSize;
    float scrollSpeed = 0.0f;
};

// Move Component

class MoveComponent : public Component {
  public:
    void update(const float &deltaTime) noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::MoveComponent;
    }

  protected:
    MoveComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 201;
    }

  public:
    // unit per second
    Vector2 velocity;
    Math::Radian rotationVelocity;
};

// Input Component

class InputComponent : public Component {
  public:
    void processInput(const uint8_t *keyState) noexcept override {
        inputResult = keyState;
    }
    virtual void update(const float &deltaTime) noexcept override;

    ComponentName getName() const noexcept override {
        return ComponentName::InputComponent;
    }
    void setKey(const uint8_t key, std::function<void(void)> behavior) {
        keymap[key] = behavior;
    }
    void setIfNotKey(std::function<void(void)> behavior) {
        behaviorInStandBy = behavior;
    }

  protected:
    InputComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 100;
    }

  protected:
    std::map<uint8_t, std::function<void(void)>> keymap;
    std::function<void(void)> behaviorInStandBy = []() {};

  private:
    const uint8_t *inputResult;
};

// AI Component

class AIComponent : public Component {
    enum State {
        Death,
        Patrol,
        Attack
    };

  public:
    void update(const float &deltaTime) noexcept override{
#warning "NOT DEFINED"
    }

    ComponentName getName() const noexcept override {
        return ComponentName::AIComponent;
    }

  protected:
    AIComponent(const std::weak_ptr<Actor> owner) noexcept
        : Component(owner) {
        updateOrder = 150;
    }
};