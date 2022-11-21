#ifndef MAPLE_6D5593289FEA48CD97ABD83F958D3576_HPP
#define MAPLE_6D5593289FEA48CD97ABD83F958D3576_HPP

#include <Windows.h>
#include <string_view>
#include "../logging/logger.hpp"

template<typename Func>
bool hook_export(std::string_view module_name, std::string_view function_name, Func *original, Func hook) {
    auto const module = reinterpret_cast<uintptr_t>(LoadLibraryA(module_name.data()));
    if (module == 0) {
        logger->error("LoadLibraryA failed");
        return false;
    }

    auto const dos_header = reinterpret_cast<IMAGE_DOS_HEADER *>(module);
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE) {
        logger->error("dos_header failed");
        return false;
    }

    auto const nt_header = (PIMAGE_NT_HEADERS)((DWORD) dos_header + (DWORD) dos_header->e_lfanew);
    if (nt_header->Signature != IMAGE_NT_SIGNATURE) {
        logger->error("nt_header failed");
        return false;
    }

    auto const export_table_offset = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    auto const export_table = (IMAGE_EXPORT_DIRECTORY*)(module + export_table_offset);

    for (auto i = 0; i < export_table->NumberOfFunctions; i++){
        auto name_offset = *(DWORD*)(module + export_table->AddressOfNames + (i * sizeof(DWORD)));
        if (strcmp((char*)(module + name_offset), function_name.data()) == 0) {
            auto ordinal = *(WORD*)(module + export_table->AddressOfNameOrdinals + (i * sizeof(WORD)));
            auto export_func = (DWORD*)(module + export_table->AddressOfFunctions + (ordinal * sizeof(DWORD)));

            std::array<uint8_t, 5> code{};

            // Create a code cave at the very top of the address space; our
            // trampoline needs to have a higher address than the target
            auto code_cave = reinterpret_cast<uint8_t *>(VirtualAlloc(nullptr, code.size(), MEM_COMMIT | MEM_RESERVE | MEM_TOP_DOWN, PAGE_READWRITE));
            if (reinterpret_cast<uintptr_t>(code_cave) < module) {
                logger->error("Code cave is too close");
                return false;
            }

            // jmp <hook>
            code[0] = 0xe9;
            *reinterpret_cast<uintptr_t *>(&code[1]) = reinterpret_cast<uintptr_t>(+hook) - reinterpret_cast<uintptr_t>(code_cave) - 5;

            memcpy(code_cave, code.data(), code.size());

            DWORD old_protect{};
            VirtualProtect(code_cave, code.size(), PAGE_EXECUTE, &old_protect);

            if (!VirtualProtect(export_func, sizeof(DWORD), PAGE_READWRITE, &old_protect)) {
                logger->error("VP failed");
                return false;
            }

            *original = (Func)(*export_func + module);
            *export_func = reinterpret_cast<uintptr_t>(code_cave) - module;

            if (!VirtualProtect(export_func, sizeof(DWORD), old_protect, &old_protect)) {
                logger->error("VP failed");
                return false;
            }

            return true;
        }
    }

    logger->error("Could not find export");
    return false;
}

#endif//MAPLE_6D5593289FEA48CD97ABD83F958D3576_HPP
