#pragma once

#include <map>
#include "System/Graphic/Types.hpp"
#include "System/Logging/Bind.hpp"

class Graphics{
  protected:
    using ColorBox = Graphic::ColorBox;
    using BasicSprite = Painter::BasicSprite;

  public:
    virtual ~Graphics()=default;
    // virtual void destroyAll() noexcept=0;

    void initialize(const Graphic::Window&) noexcept;
    void render(const Game::Time& lag) noexcept;

    virtual void draw(const ColorBox&) noexcept=0;
    virtual void clearScreen() noexcept=0;
    virtual void swapBuffer() noexcept=0;

    void request(int id, const BasicSprite&);
    void remove(int id) noexcept;

    std::string_view getTitle() noexcept{
        return windowSetting.title;
    }
    auto getWindowPosition() noexcept{
        return windowSetting.screen.position;
    }
    auto getWindowSize() noexcept{
        return windowSetting.screen.size;
    }

  public:
    static std::shared_ptr<Graphics> null;

  private:
    Logging::Bind logger={"Graphics", "base"};
    Graphic::Window windowSetting={};
    using ID=int;
    std::map<ID, BasicSprite> basicSprites;
};

class NullGraphics: public Graphics{
    virtual void draw(const BasicSprite&) noexcept{}
    virtual void clearScreen() noexcept{}
    virtual void swapBuffer() noexcept{}
};