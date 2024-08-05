#pragma once

#include <string>
#include "Info.hpp"
#include "JsonHelper.hpp"

namespace ModernBoy{
    class LevelLoader{
    private:
        Json json;

    public:
        LevelLoader(const std::string& fileName);
        WindowDesc getWindowDesc();
    };
}
