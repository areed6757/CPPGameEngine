#pragma once
#include <Core/Logger.h>

#if defined(_MSC_VER)
    #define ENGINE_DEBUG_BREAK() __debugbreak()
#else
    #include <csignal>
    #define ENGINE_DEBUG_BREAK() raise(SIGTRAP)
#endif

#ifdef _DEBUG
    #define ENGINE_ASSERT(cond, msg)                                        \
            do {                                                                 \
                if (!(cond)) {                                                   \
                    EngineLogError("Assertion failed: {} ({})",#cond, msg);      \
                    ENGINE_DEBUG_BREAK();                                        \
                }                                                                \
            } while (0)
#else
    #define ENGINE_ASSERT(cond, msg) ((void)0)
#endif