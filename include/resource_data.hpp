#ifndef __INC_RESOURCE_DATA_HPP
#define __INC_RESOURCE_DATA_HPP

#include <concepts>

template<typename T, typename Context>
concept ResourceData = requires(T t, Context ctx) {
    { t.bind(ctx) } -> std::same_as<void>;
    { t.unbind(ctx) } -> std::same_as<void>;
    { t.destroy(ctx) } -> std::same_as<void>;
};

#endif // __INC_RESOURCE_DATA_HPP