#pragma once

#include "SubEngine/GraphicsEngine.hpp"

class GraphicsEngineWithSDL: public GraphicsEngine,
    public Observer<SpriteForSDL>
{
    friend class ResourceManagerWithSDL;
  public:
    ~GraphicsEngineWithSDL();

  protected:
    GraphicsEngineWithSDL() noexcept=default;

  private:
    void onNotify(ColorRect rect) noexcept override final;
    void injectDependency() noexcept override final;
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
