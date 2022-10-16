#include "guard_pages.hpp"

#include <Windows.h>

void enable_page_guard(uintptr_t address, size_t length) {
    DWORD old_protect;
    MEMORY_BASIC_INFORMATION mbi;

    VirtualQuery(reinterpret_cast<void const *>(address), &mbi, sizeof(MEMORY_BASIC_INFORMATION));
    VirtualProtect(reinterpret_cast<void *>(address), length, mbi.Protect | PAGE_GUARD, &old_protect);
}