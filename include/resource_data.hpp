#ifndef MODERNBOY_RESOURCE_DATA_HPP
#define MODERNBOY_RESOURCE_DATA_HPP

#include <concepts>

template<typename T, typename Context>
concept ResourceData = requires(T t, Context ctx) {
    { t.bind(ctx) } -> std::same_as<void>;
    { t.unbind(ctx) } -> std::same_as<void>;
    { t.destroy(ctx) } -> std::same_as<void>;
};

#endif // MODERNBOY_RESOURCE_DATA_HPP