#ifndef MODERNBOY_CONCEPTS_HPP
#define MODERNBOY_CONCEPTS_HPP

#include "util/type_traits.hpp"

namespace ModernBoy
{
    template<typename T>
    concept OptionalType = is_std_optional_v<std::remove_cvref_t<T>>;
    template<typename T>
    concept PointerType = std::is_pointer_v<std::remove_cvref_t<T>> ||
        std::is_null_pointer_v<std::remove_cvref_t<T>>;
    template<typename T>
    concept ValueType = (!OptionalType<T>) && (!PointerType<T>);

    template <typename... Ts>
    concept AllPointer = (PointerType<Ts> && ...);
    template <typename... Ts>
    concept AllOptional = (OptionalType<Ts> && ...);
    template <typename... Ts>
    concept AllValue = (ValueType<Ts> && ...);

    template<typename T>
    concept HasShutdown = requires(T t) {
        { t.shutdown() } -> std::same_as<void>; } &&
        std::is_trivially_destructible_v<T> &&
        std::is_destructible_v<T>;
} // namespace ModernBoy

#endif // MODERNBOY_CONCEPTS_HPP