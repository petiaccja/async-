#pragma once

#include <cstdint>
#include <string_view>


namespace asyncpp::interleaving {


struct sequence_point {
    bool acquire = false;
    std::string_view name = {};
    std::string_view function = {};
    std::string_view file = {};
    uint64_t line = 0;
};


namespace impl_sp {
    void wait(sequence_point& sp);
}


#ifdef NDEUBG
    #define SEQUENCE_POINT(NAME) void()
    #define SEQUENCE_POINT_ACQUIRE(NAME) void()
    #define INTERLEAVED(EXPR) EXPR
    #define INTERLEAVED_ACQUIRE(EXPR) EXPR
#else
    #define SEQUENCE_POINT(NAME)                                  \
        {                                                         \
            static ::asyncpp::interleaving::sequence_point sp = { \
                false,                                            \
                NAME,                                             \
                __func__,                                         \
                __FILE__,                                         \
                __LINE__,                                         \
            };                                                    \
            ::asyncpp::interleaving::impl_sp::wait(sp);           \
        }                                                         \
        void()


    #define SEQUENCE_POINT_ACQUIRE(NAME)                          \
        {                                                         \
            static ::asyncpp::interleaving::sequence_point sp = { \
                true,                                             \
                NAME,                                             \
                __func__,                                         \
                __FILE__,                                         \
                __LINE__,                                         \
            };                                                    \
            ::asyncpp::interleaving::impl_sp::wait(sp);           \
        }                                                         \
        void()


    #define INTERLEAVED(EXPR)                                     \
        [&](std::string_view func) {                              \
            static ::asyncpp::interleaving::sequence_point sp = { \
                false,                                            \
                #EXPR,                                            \
                func,                                             \
                __FILE__,                                         \
                __LINE__,                                         \
            };                                                    \
            ::asyncpp::interleaving::impl_sp::wait(sp);           \
            return EXPR;                                          \
        }(__func__)


    #define INTERLEAVED_ACQUIRE(EXPR)                             \
        [&](std::string_view func) {                              \
            static ::asyncpp::interleaving::sequence_point sp = { \
                true,                                             \
                #EXPR,                                            \
                func,                                             \
                __FILE__,                                         \
                __LINE__,                                         \
            };                                                    \
            ::asyncpp::interleaving::impl_sp::wait(sp);           \
            return EXPR;                                          \
        }(__func__)
#endif

} // namespace asyncpp::interleaving
