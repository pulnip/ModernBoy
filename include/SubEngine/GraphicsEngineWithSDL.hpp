#pragma once

#include "GraphicsEngine.hpp"

class GraphicsEngineWithSDL : public GraphicsEngine, public Observer<SDL_Sprite> {
  public:
    ~GraphicsEngineWithSDL();

    void prepareRendering() noexcept override {
        initBackground();
    }
    void finalizeRendering() noexcept override {
        changeColorBuffer();
    }

  protected:
    GraphicsEngineWithSDL(const OwnerRef owner) noexcept : GraphicsEngine(owner) {}

  private:
    void postConstruct() noexcept override;

    void onNotify(ColorRect rect) noexcept;
    void onNotify(SDL_Sprite sprite) noexcept;

    void initBackground() noexcept;
    void changeColorBuffer() noexcept;

  private:
    // SDL이 생성한 윈도우
    std::shared_ptr<SDL_Window *> window;
    std::shared_ptr<SDL_Renderer *> renderer;
};
