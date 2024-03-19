#pragma once

#include <string>

namespace Logging{
    enum class Level{
        DEBUG=0,
        INFO=1,
        WARNING=2,
        ERROR=3,
        CRITICAL=4
    };

    struct String{
        std::string type="default";
        std::string data="NO MESSAGE";
        operator std::string(){ return data; }
    };

    struct Integer{
        std::string type="default";
        int data=0;
        operator int(){ return data; }
    };

    struct Double{
        std::string type="default";
        double data=0.0;
        operator double(){ return data; }
    };

    struct Info{
        std::string type="NO_TYPE", name="NO_NAME";
    };
}