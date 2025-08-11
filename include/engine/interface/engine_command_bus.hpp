#ifndef MODERNBOY_INTERFACE_ENGINECOMMANDBUS_HPP
#define MODERNBOY_INTERFACE_ENGINECOMMANDBUS_HPP

#include <mutex>
#include <variant>
#include <vector>

namespace ModernBoy::Interface
{
    enum class CursorMode{
        RELATIVE,
        ABSOLUTE
    };

    struct SetCursorMode{
        CursorMode mode;
    };

    using EngineCommand = std::variant<
        SetCursorMode>;

    class EngineCommandBus{
    public:
        EngineCommandBus() = default;
        ~EngineCommandBus() = default;

        void write(EngineCommand command);

        std::vector<EngineCommand> drainCommands();

    private:
        std::vector<EngineCommand> commands;

        std::mutex mtx;
    };
}

#endif // MODERNBOY_INTERFACE_ENGINECOMMANDBUS_HPP