#include "bootstrap.hpp"

#include "../main.hpp"
#include "../maple/addresses.hpp"
#include "../util/hooking/guard_pages.hpp"
#include "../util/logging/logger.hpp"

#include <Windows.h>
#include <cstdint>

LONG CALLBACK bootstrap_hook(PEXCEPTION_POINTERS ex);

PVOID bootstrap_hook_handle = nullptr;
bool bootstrapped = false;

void install_bootstrap_hook() {
    uint32_t const first_exception_handler = 1;
    bootstrap_hook_handle = AddVectoredExceptionHandler(first_exception_handler, bootstrap_hook);

    enable_page_guard(address::func::WinMain, 5);
}

LONG CALLBACK bootstrap_hook(PEXCEPTION_POINTERS ex) {
    if (bootstrapped) {
        logger->debug("Removing bootstrap hook");
        RemoveVectoredExceptionHandler(bootstrap_hook_handle);
    }

    if (ex->ExceptionRecord->ExceptionCode == EXCEPTION_GUARD_PAGE) {
        auto const eip = ex->ContextRecord->Eip;

        if (eip == address::func::WinMain) {
            // Provide some command-line arguments:
            //   0: IP address of login server
            //   1: port of login server
            //   2: username for auto-login
            //   3: password, skips Wizet logo if present
            auto WinMain_lpCmdLine = reinterpret_cast<char const **>(ex->ContextRecord->Esp + 0xc);
            *WinMain_lpCmdLine = "10.144.0.0 8484";// foo bar";

            // The auto-login feature is pretty buggy. Random crashes depending
            // on how fast the server responds:

            // seg000:0049AAA8                 mov     eax, ds:CWvsContext_instance
            // seg000:0049AAAD                 mov     ecx, [eax+20h]
            // seg000:0049AAB0                 mov     eax, [eax+91Ch]
            // seg000:0049AAB6                 mov     eax, [eax+ecx*4]              <<< invalid ptr

            init();

            bootstrapped = true;
        }

        ex->ContextRecord->EFlags |= 0x100ui32;

        return EXCEPTION_CONTINUE_EXECUTION;
    } else if (ex->ExceptionRecord->ExceptionCode == EXCEPTION_SINGLE_STEP) {
        if (!bootstrapped) {
            // We need to re-register our hook in case this exception handler fired
            // while ASProtect was still unpacking the game
            enable_page_guard(address::func::WinMain, 5);
        }

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}