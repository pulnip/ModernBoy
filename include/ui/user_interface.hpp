#ifndef MODERNBOY_UI_USER_INTERFACE_HPP
#define MODERNBOY_UI_USER_INTERFACE_HPP

#include <unordered_map>
#include <SDL3/SDL_video.h>
#include "ui/controller.hpp"

namespace ModernBoy::UI
{
    using ControllerID = uint32_t;

    class UserInterface{
    public:
        UserInterface(AppState& app, SDL_Window* window);

        template<typename ControllerType, typename ...Args>
        ControllerID emplace(Args&&... args){
            auto id = issueID();
            controllers.emplace(id, ControllerType(
                std::forward<Args>(args)...
            ));
            return id;
        }
        template<typename Control, typename... Args>
        ControlID emplace(ControllerID id, Args&&... args) {
            auto ctrlID = issueID();

            std::visit([&](auto& controller){
                controller.template emplace<Control>(
                    ctrlID, std::forward<Args>(args)...);
            }, controllers.at(id));
            control2Controller.emplace(ctrlID, id);
            return ctrlID;
        }

        size_t yield_count() const noexcept;

        void onFrameStart();
        Generator<void> updateTask(DeltaTime);
        void onFrameEnd();

        void handleEvent(Event event);

    private:
        uint32_t issueID() const noexcept;

        AppState& app;

        std::unordered_map<ControllerID, Controller> controllers;
        std::unordered_map<ControlID, ControllerID> control2Controller;
    };
} // namespace ModernBoy::UI

#endif // MODERNBOY_UI_USER_INTERFACE_HPP