#include "string_pool.hpp"

#include "../maple/string.hpp"
#include "../util/memory/patch.hpp"

auto const log_calls = false;

auto const StringPool_GetString = 0x518768;

void hook_string_pool() {
    Patch(StringPool_GetString).int3();
}

bool handle_string_pool_hooks(uintptr_t addr, PCONTEXT ctx) {
    if (addr == StringPool_GetString) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const pool = ctx->Ecx;
        auto const output = *reinterpret_cast<uintptr_t **>(ctx->Esp + 4);
        auto const index = *reinterpret_cast<uint32_t *>(ctx->Esp + 8);

        if (log_calls) {
            logger->trace("StringPool::GetString(0x{:x}, 0x{:8x}, {}) from 0x{:x}",
                          pool, reinterpret_cast<uintptr_t>(output), index, ret_addr);
        }

#define REPLACE_STR(INDEX, STR)                     \
    case (INDEX): {                                 \
        static StaticZXString zxstring(STR, 2);     \
        static char const *ptr = zxstring.to_str(); \
        replacement = &ptr;                         \
        break;                                      \
    }
#define REPLACE_STR_DYN(INDEX, STR)                         \
    case (INDEX): {                                         \
        static auto zxstring = ZXString<char>::create(STR); \
        static char const *ptr = zxstring.str;              \
        replacement = &ptr;                                 \
        break;                                              \
    }

        char const **replacement = nullptr;

        switch (index) {
            // "MapleStory Beta": window title
            REPLACE_STR_DYN(783, fmt::format("MapleStory | {}", GetCurrentProcessId()).c_str())
            // "Ver %d.%d": version on login screen
            REPLACE_STR(2133, "Ver 13.37")
            // "Welcome to MapleStory": message displayed in chat when logging in
            REPLACE_STR(553, "yo!")
        }

#undef REPLACE_STR

        if (replacement != nullptr) {
            *output = reinterpret_cast<uintptr_t>(*replacement);
            ctx->Eax = reinterpret_cast<uintptr_t>(replacement);

            // seg000:005187C9                 retn    8
            ctx->Eip = 0x5187c9;
        } else {
            // seg000:00518768                 mov     eax, offset loc_61440F
            // seg000:0051876D                 call    __EH_prolog
            ctx->Eax = 0x61440F;
            ctx->Eip = 0x51876D;
        }

        return true;
    }

    return false;
}