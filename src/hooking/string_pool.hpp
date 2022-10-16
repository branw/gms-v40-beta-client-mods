#ifndef MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP
#define MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP

#include <Windows.h>
#include <cstdint>

void hook_string_pool();

bool handle_string_pool_hooks(uintptr_t addr, PCONTEXT ctx);

#endif//MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP
