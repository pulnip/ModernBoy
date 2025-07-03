#ifndef MODERNBOY_FUNC_TRAIT_HPP
#define MODERNBOY_FUNC_TRAIT_HPP

#include <tuple>

namespace ModernBoy{
    template<typename T>
    struct fn_traits;

    template<typename R, typename... Args>
    struct fn_traits<R (*)(Args...)>{
        using args = std::tuple<Args...>;
    };

    template<typename C, typename R, typename... Args>
    struct fn_traits<R (C::*)(Args...) const>{
        using args = std::tuple<Args...>;
    };

    template<typename F>
    using fn_args_t = typename fn_traits<
            decltype(&std::decay_t<F>::operator())
        >::args;

    template<typename Tuple>
    struct decay_tuple;

    template<typename... Ts>
    struct decay_tuple<std::tuple<Ts...>>{
        using type = std::tuple<std::decay_t<Ts>...>;
    };

    template<typename F>
    using fn_decayed_args_t = typename decay_tuple<
        fn_args_t<F>>::type;
}

#endif // MODERNBOY_FUNC_TRAIT_HPP