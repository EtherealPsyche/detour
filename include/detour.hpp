#pragma once

namespace detour {
    struct result {
        enum class code {
            success,
            not_hooked,
            already_hooked,
            not_initialized,
            already_initialized,
            initialize_failed,
            already_enabled,
            already_disabled,
            invalid_argument,
            internal_error,
        };

        constexpr result(code m_code) : m_code(m_code) {}

        auto status() const -> code { return m_code; }
        auto is_ok() const -> bool { return m_code == code::success; }
        auto is_err() const -> bool { return m_code != code::success; }
        auto string() const -> const char *;

      private:
        code m_code;

        friend auto format_as(const result &self) -> const char * { return self.string(); }
    };

    auto create(void *target, void *replace, void **origin) -> result;
    auto remove(void *target) -> result;

    template <typename Return, typename... Args>
    auto create(void *target, Return (*replace)(Args...), Return (**origin)(Args...)) -> result {
        return detour::create(target, (void *)replace, (void **)origin);
    }

    template <typename Return, typename... Args>
    auto create(Return (*target)(Args...), Return (*replace)(Args...), Return (**origin)(Args...)) -> result {
        return detour::create((void *)target, (void *)replace, (void **)origin);
    }

#if defined(_WIN32) || defined(_WIN64)
    auto enable(void *target) -> result;
    auto disable(void *target) -> result;
#endif
}
