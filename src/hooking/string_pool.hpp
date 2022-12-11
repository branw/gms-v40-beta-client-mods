#ifndef MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP
#define MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP

#include <Windows.h>
#include <cstdint>
#include <unordered_map>

#include "../maple/string.hpp"

extern bool log_string_pool_reads;

void hook_string_pool();

bool handle_string_pool_hooks(uintptr_t addr, PCONTEXT ctx);

std::unordered_map<uint32_t, ZXString<char>> &string_pool_replacements();

#endif//MAPLE_52CDDBE55FB24092BE89FF3607AEC28C_HPP
