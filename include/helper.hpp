#pragma once

#include <chrono>
#include <functional>
#include <print>
#include <source_location>
#include <string>
#include <d3dcommon.h>
#include <winnt.h>
#include "Concepts.hpp"

#ifndef nameof
#define nameof(x) (#x)
#endif

namespace ModernBoy{
    class Exception: public std::exception{
        std::string msg;

      public:
        Exception(const std::string& msg="Default Exception",
            const std::source_location& sl=std::source_location::current())
        : msg(std::format("{} ({} {}:{})", msg, 
            sl.file_name(), sl.line(), sl.column()
        )){}

        const char* what() const noexcept override{
            return msg.c_str();
        }
    };

    constexpr bool is_neg(std::signed_number auto n){ return n < 0; }
    constexpr bool is_non0(std::integral auto i){ return i != 0; }
    constexpr bool is_null(std::pointer auto p){ return p == nullptr; }

    constexpr void throwIf(bool pred,
        const std::string& msg="Default Exception",
        const std::source_location& sl=std::source_location::current()
    ){
        if(pred) throw Exception(msg, sl);
    }
    template<typename T>
    constexpr void throwIfFailed(T code, std::function<bool(T)> pred,
        const std::string& msg="Default Exception",
        const std::source_location& sl=std::source_location::current()
    ){
        throwIf(pred(code), msg, sl);
    }
    inline void DX_throwIf(HRESULT hr,
        const std::source_location& sl=std::source_location::current()
    ){
        throwIf(is_neg(hr), "DirectX Exception", sl);
    }
    inline void SDL_throwIf(int code,
        const std::source_location& sl=std::source_location::current()
    ){
        throwIf(is_neg(code), "SDL Exception", sl);
    }
    // throwIf function for shader compiler
    inline void SC_throwIf(HRESULT hr, ID3DBlob* errorBlob,
        const std::source_location& sl=std::source_location::current()
    ){
        if(is_neg(hr)){
            throwIf(is_non0(hr&D3D11_ERROR_FILE_NOT_FOUND),
                nameof(D3D11_ERROR_FILE_NOT_FOUND), sl
            );
            throwIf(not is_null(errorBlob),
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
