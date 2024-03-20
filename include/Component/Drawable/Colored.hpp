#pragma once

#include "System/Logging/Bind.hpp"
#include "Component/Drawable/Drawable.hpp"
#include "Component/Drawable/Types.hpp"

class Colored final: public Drawable{
    using Color=Skin::TrueColor;
    Colored(Skin::TrueColor color) noexcept:
        color(color), Drawable(500){}

    void drawAt(const Pos&) noexcept override final;
    void setColor(const Color& color) noexcept{
        this->color=color;
    }    

  private:
    Logging::Bind logger={"Colored", std::to_string(id)};
    Color color;
};