#ifndef MODERNBOY_UI_CONTROL_HPP
#define MODERNBOY_UI_CONTROL_HPP

#include <string>
#include <variant>
#include <vector>
#include "alias.hpp"
#include "ui/event.hpp"

namespace ModernBoy::UI
{
    struct CheckBox{
        std::string label;
        bool checked;

        void draw();
        void OnEvent(Event);
    };
    struct RadioButton{
        std::vector<std::string> labels;
        Index index;

        void draw();
        void OnEvent(Event);
    };
    struct Slider{
        std::string label;
        const float minValue, maxValue;
        float value;

        void draw();
        void OnEvent(Event);
    };
    struct SameLine{
        void draw();
        void OnEvent(Event);
    };

    using Control = std::variant<
        CheckBox, RadioButton,
        Slider, SameLine
    >;
} // namespace ModernBoy::UI

#endif // MODERNBOY_UI_CONTROL_HPP