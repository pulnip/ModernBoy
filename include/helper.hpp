#pragma once

#include <chrono>
#include <functional>
#include <print>
#include <source_location>
#include <string>
#include <d3dcommon.h>
#include <winnt.h>

#ifndef nameof
#define nameof(x) (#x)
#endif

namespace ModernBoy{
    struct Exception{
        std::string msg;
        std::string fileName;
        unsigned int lineNumber;
    };

    template<typename T> constexpr bool is_neg(T t){ return t<0; }

    template<typename T>
    constexpr void throwIfFailed(T code, std::function<bool(T)> pred,
        const std::source_location& sl=std::source_location::current()
    ){
        if(pred(code)){
            throw Exception{std::to_string(code), sl.file_name(), sl.line()};
        }
    }
    constexpr void throwIfTrue(bool pred, const std::string& msg,
        const std::source_location& sl=std::source_location::current()
    ){
        if(pred){
            throw Exception{msg, sl.file_name(), sl.line()};
        }
    }
    inline void DX_throwIf(HRESULT hr,
        const std::source_location& sl=std::source_location::current()
    ){
        throwIfFailed<HRESULT>(hr, is_neg<HRESULT>, sl);
    }
    inline void SDL_throwIf(int code,
        const std::source_location& sl=std::source_location::current()
    ){
        throwIfFailed<int>(code, is_neg<int>, sl);
    }
    inline void SC_throwIf(HRESULT hr, ID3DBlob* errorBlob,
        const std::source_location& sl=std::source_location::current()
    ){
        if(is_neg(hr)){
            throwIfTrue((hr&D3D11_ERROR_FILE_NOT_FOUND) != 0,
                nameof(D3D11_ERROR_FILE_NOT_FOUND), sl
            );
            throwIfTrue(errorBlob != nullptr,
                (char*)errorBlob->GetBufferPointer(), sl
            );
        }
    }

    constexpr void printIf(bool pred, const std::string& msg){
        if(pred) std::println("{:s}", msg);
    }

    class TimeGuard{
        using Clock=std::chrono::high_resolution_clock;
        using TimePoint=std::chrono::time_point<Clock>;

        std::string caseName;
        TimePoint start=Clock::now();
        
      public:
        TimeGuard(const std::string& caseName):caseName(caseName){}
        ~TimeGuard(){
            std::chrono::duration<double> elapsed{(Clock::now()-start)};
            std::println("{:s}: {}", caseName, elapsed.count());
        }
    };
}
