#include <stdexcept>
#include "engine/script/as_stream.hpp"

using namespace ModernBoy;
using namespace ModernBoy::Script;

StreamWrapper::StreamWrapper(const std::string& file,
    std::ios_base::openmode mode)
:stream(file, mode){
    if(!stream.is_open() || stream.fail() || stream.bad() )
        throw std::runtime_error(file+" not exist!");
}

int StreamWrapper::Write(const void* data, asUINT size){
    stream.write(static_cast<const char*>(data), size);
    return static_cast<int>(size);
}
int StreamWrapper::Read(void* buf, asUINT size){
    stream.read(static_cast<char*>(buf), size);
    return static_cast<int>(size);
}
