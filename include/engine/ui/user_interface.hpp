#ifndef MODERNBOY_UI_USER_INTERFACE_HPP
#define MODERNBOY_UI_USER_INTERFACE_HPP

#include <unordered_map>
#include <SDL3/SDL_video.h>
#include "engine/interface.hpp"
#include "engine/ui/controller.hpp"

namespace ModernBoy::UI
{
    using ControllerID = uint32_t;

    class UserInterface{
    public:
        UserInterface(SDL_Window* window,
            Render::Renderer& renderer,
            Engine& engine);

        TaskTime expectedExecTime();
        void onFrameStart();
        void update(DeltaTime);
        // Generator<void> update(DeltaTime);
        void onFrameEnd();

        void handleEvent(Event event);

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

        const TaskPolicy policy{
            .effective_window_size = 10,
            .min_interval = std::chrono::microseconds(1000/60),
            .patience = std::chrono::milliseconds(0)
        };

    private:
        void updateEMA(TaskTime);

        uint32_t issueID() const noexcept;

        Render::Renderer& renderer;
        Engine& engine;

        std::unordered_map<ControllerID, Controller> controllers;
        std::unordered_map<ControlID, ControllerID> control2Controller;

        TaskTime ema;
    };
} // namespace ModernBoy::UI

#endif // MODERNBOY_UI_USER_INTERFACE_HPP