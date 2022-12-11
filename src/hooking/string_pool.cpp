#include "string_pool.hpp"
#include "../util/memory/patch.hpp"

bool log_string_pool_reads = false;

auto const StringPool_GetString = 0x518768;

std::unordered_map<uint32_t, ZXString<char>> &string_pool_replacements() {
    static std::unordered_map<uint32_t, ZXString<char>> replacements{};

    if (replacements.empty()) {
        // "MapleStory Beta": window title
        replacements[783] = ZXString<char>::create(fmt::format("MapleStory | {}", GetCurrentProcessId()));
        // "Ver %d.%d": version on login screen
        replacements[2133] = ZXString<char>::create("Ver 13.37");
        // "Welcome to MapleStory": message displayed in chat when logging in
        replacements[553] = ZXString<char>::create("yo!");
    }

    return replacements;
}

void hook_string_pool() {
    Patch(StringPool_GetString).int3();
}

bool handle_string_pool_hooks(uintptr_t addr, PCONTEXT ctx) {
    if (addr == StringPool_GetString) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const pool = ctx->Ecx;
        auto const output = *reinterpret_cast<uintptr_t **>(ctx->Esp + 4);
        auto const index = *reinterpret_cast<uint32_t *>(ctx->Esp + 8);

        if (log_string_pool_reads) {
            logger->debug("StringPool::GetString(0x{:x}, 0x{:8x}, {}) from 0x{:x}",
                          pool, reinterpret_cast<uintptr_t>(output), index, ret_addr);
        }

        auto &replacements = string_pool_replacements();
        auto iter = replacements.find(index);
        if (iter != replacements.end()) {
            auto const replacement = &iter->second.str;

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