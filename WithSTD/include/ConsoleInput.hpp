#pragma once

#include <vector>
#include <map>
#include "System/Logging/Bind.hpp"
#include "System/Input/InputReceiver.hpp"

class ConsoleInput: public InputReceiver{
    void pollEvent() override;
    void subscribe(const Input::Event&, Receiver recv) override;

  private:
    Logging::Bind logger={"Input", "Console"};
    std::map<Input::Event, std::vector<Receiver>> keyMap;
};
