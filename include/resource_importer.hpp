#ifndef MODERNBOY_RESOURCE_IMPORTER_HPP
#define MODERNBOY_RESOURCE_IMPORTER_HPP

#include <concepts>
#include <string>

namespace ModernBoy{
    template<typename T, typename R>
    concept ResourceImporter = requires(T t,
        const std::string& fileName
    ){
        { t.import(fileName) } -> std::same_as<R>;
    };
}

#endif // MODERNBOY_RESOURCE_IMPORTER_HPP