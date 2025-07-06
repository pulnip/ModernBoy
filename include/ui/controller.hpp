#ifndef MODERNBOY_UI_CONTROLLER_HPP
#define MODERNBOY_UI_CONTROLLER_HPP

#include <unordered_map>
#include <variant>
#include "fwd.hpp"
#include "util/generator.hpp"
#include "ui/control.hpp"
#include "ui/event.hpp"

namespace ModernBoy::UI
{
    class UserInterface;

    class MenuController{
    public:
        MenuController();

        template<typename Control, typename ...Args>
        ControlID emplace(ControlID id, Args&&... args){
            controls.emplace(id, Control(
                std::forward<Args>(args)...));
            return id;
        }

        void update(DeltaTime);

        void OnEvent(Event event);

    private:
        std::unordered_map<ControlID, Control> controls;
    };

    using Controller = std::variant<
        MenuController
    >;
} // namespace ModernBoy::UI


#endif // MODERNBOY_UI_CONTROLLER_HPP