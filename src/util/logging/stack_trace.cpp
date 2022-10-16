#include <Windows.h>
#include <fmt/core.h>
#include <utility>

#define _NO_CVCONST_H

#include "logger.hpp"
#include "stack_trace.hpp"

#include <dbghelp.h>

void install_exception_logger() {
    uint32_t const first_exception_handler = 1;
    AddVectoredExceptionHandler(first_exception_handler, exception_logger_hook);
}

void log_stack_trace(LogLevel log_level) {
    CONTEXT context{0};
    RtlCaptureContext(&context);

    log_stack_trace(log_level, context);
}

void log_stack_trace(LogLevel log_level, CONTEXT &context) {
    static auto const process = GetCurrentProcess();
    if (static auto init = false; !std::exchange(init, true)) {
        SymInitialize(process,
                      "srv*C:\\Symbols*https://msdl.microsoft.com/download/symbols",
                      true);
    }

    auto const thread = GetCurrentThread();

    STACKFRAME stack = {
            .AddrPC = {.Offset = context.Eip, .Mode = AddrModeFlat},
            .AddrFrame = {.Offset = context.Ebp, .Mode = AddrModeFlat},
            .AddrStack = {.Offset = context.Esp, .Mode = AddrModeFlat},
    };

    logger->log(log_level, "EIP {:8x} ESP {:8x}", stack.AddrPC.Offset, stack.AddrStack.Offset);

    for (auto frame_index = 0;; frame_index++) {
        SetLastError(0);
        if (!StackWalk(IMAGE_FILE_MACHINE_I386,
                       process, thread, &stack, &context,
                       nullptr,
                       SymFunctionTableAccess, SymGetModuleBase,
                       nullptr)) {
            if (auto const error_code = GetLastError(); error_code != 0) {
                logger->log(log_level, "Stack walk failed (error code {})", error_code);
            }
            break;
        }

        if (auto const addr = stack.AddrPC.Offset; addr) {
            if (addr > 0x8000'0000) {
                return;
            }

            HMODULE module;
            uintptr_t module_offset = 0;
            char module_name[MAX_PATH]{0};
            auto const addr_str = reinterpret_cast<LPCSTR>(addr);
            if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, addr_str, &module) &&
                module != nullptr) {
                module_offset = addr - reinterpret_cast<uintptr_t>(module);
                GetModuleFileNameA(module, module_name, MAX_PATH - 1);
            }

            logger->log(log_level, "#{:<3} {:08x} ({}+{:x})",
                        frame_index, stack.AddrPC.Offset, module_name, module_offset);

            auto const sym_name_size = 256;
            auto const sym_size = sizeof(IMAGEHLP_SYMBOL) + sym_name_size;
            std::aligned_storage_t<sym_size, alignof(IMAGEHLP_SYMBOL)> sym_buffer{0};
            auto &sym = reinterpret_cast<IMAGEHLP_SYMBOL &>(sym_buffer);
            sym.SizeOfStruct = sym_size;
            sym.MaxNameLength = sym_name_size;

            if (SymGetSymFromAddr(process, stack.AddrPC.Offset, nullptr, &sym)) {
                logger->log(log_level, "      {}", sym.Name);
            }

            IMAGEHLP_LINE line = {.SizeOfStruct = sizeof(line)};
            DWORD displacement;

            if (SymGetLineFromAddr(process, stack.AddrPC.Offset, &displacement, &line)) {
                logger->log(log_level, "      ({}:{})", line.FileName, line.LineNumber);
            }
        }
    }
}

LONG WINAPI exception_logger_hook(PEXCEPTION_POINTERS ex) {
    auto const code = ex->ExceptionRecord->ExceptionCode;
    // Skip C++ and debugger exceptions
    if (code == 0xe06d7363 || code == EXCEPTION_GUARD_PAGE || code == EXCEPTION_SINGLE_STEP) {
        return EXCEPTION_CONTINUE_SEARCH;
    }

    auto const addr = reinterpret_cast<uintptr_t>(ex->ExceptionRecord->ExceptionAddress);

    HMODULE module;
    uintptr_t offset = 0;
    char module_name[MAX_PATH]{0};
    auto const addr_str = reinterpret_cast<LPCSTR>(addr);
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, addr_str, &module) &&
        module != nullptr) {
        offset = addr - reinterpret_cast<uintptr_t>(module);
        GetModuleFileNameA(module, module_name, MAX_PATH - 1);
    }

    if (logger == nullptr) {
        MessageBox(nullptr,
                   fmt::format("Exception {:08x} at {:08x} ({}+{:08x})", code, addr,
                               module_name, offset)
                           .c_str(),
                   "Niko Exception", MB_ICONERROR | MB_OK);
    } else if (code == DBG_PRINTEXCEPTION_C && ex->ExceptionRecord->NumberParameters == 2) {
        //auto len = static_cast<ULONG>(ex->ExceptionRecord->ExceptionInformation[0]);
        std::string str{reinterpret_cast<PCSTR>(ex->ExceptionRecord->ExceptionInformation[1])};
        str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
        logger->debug("OutputDebugString: {}", str);
    } else {
        auto const level = (code < 0x80000000) ? LogLevel::Trace : LogLevel::Error;

        logger->log(level, "Exception {:08x} at {:08x} ({}+{:08x})", code, addr,
                    module_name, offset);

        log_stack_trace(level, *ex->ContextRecord);
    }

    // Continue down the exception handler chain
    return EXCEPTION_CONTINUE_SEARCH;
}