#include "detour.hpp"

#ifdef __ANDROID__
#include "dobby.h"
#elif defined(_WIN32) || defined(_WIN64)
#include "MinHook.h"
#else
#error "unsupported compile target"
#endif

namespace detour {
    auto result::string() const -> const char * {
        switch (m_code) {
        case code::success:
            return "success";
        case code::not_hooked:
            return "not hooked";
        case code::already_hooked:
            return "already hooked";
        case code::not_initialized:
            return "not initialized";
        case code::already_initialized:
            return "already initialized";
        case code::initialize_failed:
            return "initialize failed";
        case code::already_enabled:
            return "already enabled";
        case code::already_disabled:
            return "already disabled";
        case code::invalid_argument:
            return "invalid argument";
        case code::internal_error:
            return "internal error";
        default:
            return "unknown";
        }
    }

#if defined(_WIN32) || defined(_WIN64)
    static auto convert_code(MH_STATUS code) -> result::code {
        if (code == MH_OK)
            return result::code::success;
        if (code == MH_ERROR_ALREADY_INITIALIZED)
            return result::code::already_initialized;
        if (code == MH_ERROR_NOT_INITIALIZED)
            return result::code::not_initialized;
        if (code == MH_ERROR_ALREADY_CREATED)
            return result::code::already_hooked;
        if (code == MH_ERROR_NOT_CREATED)
            return result::code::not_hooked;
        if (code == MH_ERROR_ENABLED)
            return result::code::already_enabled;
        if (code == MH_ERROR_DISABLED)
            return result::code::already_disabled;
        return result::code::internal_error;
    }
#endif
}

auto detour::create(void *target, void *replace, void **origin) -> result {
    if (target == nullptr || replace == nullptr)
        return result::code::invalid_argument;
#ifdef __ANDROID__
    int result = DobbyHook(target, replace, origin);
    if (result == 0)
        return result::code::success;
    return result::code::internal_error;
#elif defined(_WIN32) || defined(_WIN64)
    static auto init_status = MH_Initialize();
    if (init_status != MH_OK)
        return result::code::initialize_failed;
    auto create = MH_CreateHook(target, replace, origin);
    if (create != MH_OK)
        return detour::convert_code(create);
    auto enable = MH_EnableHook(target);
    return detour::convert_code(enable);
#endif
}

auto detour::remove(void *target) -> result {
    if (target == nullptr)
        return result::code::invalid_argument;
#ifdef __ANDROID__
    int result = DobbyDestroy(target);
    if (result == 0)
        return result::code::success;
    if (result == -1)
        return result::code::not_hooked;
    return result::code::internal_error;
#elif defined(_WIN32) || defined(_WIN64)
    auto result = MH_RemoveHook(target);
    return detour::convert_code(result);
#endif
}

#if defined(_WIN32) || defined(_WIN64)
auto detour::enable(void *target) -> result {
    if (target == nullptr)
        return result::code::invalid_argument;
    auto result = MH_EnableHook(target);
    return detour::convert_code(result);
}

auto detour::disable(void *target) -> result {
    if (target == nullptr)
        return result::code::invalid_argument;
    auto result = MH_DisableHook(target);
    return detour::convert_code(result);
}
#endif