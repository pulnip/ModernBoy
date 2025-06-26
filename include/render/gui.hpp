#ifndef MODERNBOY_RENDER_GUI_HPP
#define MODERNBOY_RENDER_GUI_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include "fwd.hpp"

namespace ModernBoy{
    template<typename T>
    using Observer = std::function<void(T)>;
    template<typename T>
    using Observers = std::unordered_map<ObserverID, Observer<T>>;

#define UI_ELM_DECL(e) \
public: \
    [[nodiscard]] ObserverID subscribe##e( \
        const Observer<decltype(e)>& o) noexcept; \
    void notify##e() noexcept; \
    void unsubscribe##e(ObserverID id); \
private: \
    Observers<decltype(e)> e##Observers; \
    mutable std::mutex e##Mutex;

    class UI{
    private:
        ObserverID id_seed = 0;

    public:
        bool drawAsWire=false;
        // ToDo. sync when init
        float normalScale = 0.0f;
        int lightType=0;
        float rimPower=1.0f;
        float rimStrength=0.0f;

        void update();

        UI_ELM_DECL(drawAsWire)
        UI_ELM_DECL(normalScale)
        UI_ELM_DECL(lightType)
        UI_ELM_DECL(rimPower)
        UI_ELM_DECL(rimStrength)
    };
} // namespace ModernBoy

#undef UI_ELM_DECL

#endif // MODERNBOY_RENDER_GUI_HPP