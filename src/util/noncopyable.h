#pragma once

namespace jnr {

    class NonCopyable {
    protected:
        constexpr NonCopyable() = default;

        ~NonCopyable() = default;

    public:
        NonCopyable(const NonCopyable &) = delete;

        NonCopyable &operator=(const NonCopyable &) = delete;
    };

}