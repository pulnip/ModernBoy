#ifndef MODERNBOY_SCRIPT_AS_STREAM_HPP
#define MODERNBOY_SCRIPT_AS_STREAM_HPP

#include <fstream>
#include <string>
#include <angelscript.h>

namespace ModernBoy::Script
{
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
} // namespace ModernBoy::Script

#endif // MODERNBOY_SCRIPT_AS_STREAM_HPP