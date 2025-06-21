#ifndef MODERNBOY_UTIL_AS_STREAM_HPP
#define MODERNBOY_UTIL_AS_STREAM_HPP

#include <fstream>
#include <string>
#include <angelscript.h>

namespace ModernBoy{ namespace Util{
    constexpr auto STREAM_READ  = std::ios::binary | std::ios::in;
    constexpr auto STREAM_WRITE = std::ios::binary | std::ios::out | std::ios::trunc;

    class StreamWrapper: public asIBinaryStream{
    public:
        StreamWrapper(const std::string& file,
            std::ios_base::openmode mode = STREAM_READ);

        int Write(const void* ptr, asUINT size) override;
        int Read(void*, asUINT) override;

    private:
        std::fstream stream;
    };
}}

#endif // MODERNBOY_UTIL_AS_STREAM_HPP