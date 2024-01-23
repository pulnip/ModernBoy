#pragma once

#include "SubEngine/GraphicsEngine.hpp"

class GraphicsEngineWithSDL final: public GraphicsEngine,
    public Observer<SpriteForSDL>
{
    friend class ResourceManagerWithSDL;
  public:
    GraphicsEngineWithSDL() noexcept=default;
    ~GraphicsEngineWithSDL();

  private:
    void onNotify(ColorRect rect) noexcept override final;
    void setAttribute() noexcept override final;
    void prepareRendering() noexcept override final{ initBackground(); }
    void finalizeRendering() noexcept override final{ changeColorBuffer(); }

    void onNotify(SpriteForSDL sprite) noexcept;
    void initBackground() noexcept;
    void changeColorBuffer() noexcept;

  private:
    // SDL이 생성한 윈도우
    std::shared_ptr<SDL_Window*> window;
    std::shared_ptr<SDL_Renderer*> renderer;
};
