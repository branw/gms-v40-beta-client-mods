#define NOMINMAX

#include "dinput8.hpp"
#include "hooking/bootstrap.hpp"
#include "hooking/graphics.hpp"
#include "hooking/packets.hpp"
#include "hooking/string_pool.hpp"
#include "maple/addresses.hpp"
#include "util/logging/console_logger_sink.hpp"
#include "util/logging/file_logger_sink.hpp"
#include "util/logging/stack_trace.hpp"
#include "util/memory/patch.hpp"

#include <Windows.h>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <winnt.h>
#include <winternl.h>

[[maybe_unused]] BOOL WINAPI DllMain(HINSTANCE module, DWORD call_reason, LPVOID) {
    if (call_reason == DLL_PROCESS_ATTACH) {
        // Ignore thread notifications
        DisableThreadLibraryCalls(module);

        // Save a handle to ourselves so we can unload later
        // static auto const module_handle = module;

        // Create a main thread for doing the major work to prevent deadlocking the
        // process while still in LoadLibrary
        CreateThread(
                nullptr, 0,
                [](PVOID) -> DWORD {
                    dinput8::init();

                    // Because we're being loaded before the program starts,
                    // we need to wait for ASProtect to finish unpacking the
                    // game
                    install_bootstrap_hook();

                    // 2003 state-of-the-art debugger detection
                    auto const teb = reinterpret_cast<PTEB>(__readfsdword(offsetof(NT_TIB, Self)));
                    teb->ProcessEnvironmentBlock->BeingDebugged = 0;

                    // FreeLibraryAndExitThread(module_handle, EXIT_SUCCESS);
                    return 0;
                },
                nullptr, 0, nullptr);
    }

    return TRUE;
}

LONG CALLBACK exception_handler(PEXCEPTION_POINTERS ex) {
    auto const addr = reinterpret_cast<uintptr_t>(ex->ExceptionRecord->ExceptionAddress);
    auto const ctx = ex->ContextRecord;

    if (
            handle_string_pool_hooks(addr, ctx) ||
            handle_out_packet_hooks(addr, ctx) ||
            handle_in_packet_hooks(addr, ctx)) {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return exception_logger_hook(ex);
}

void install_vectored_exception_handler() {
    uint32_t const first_exception_handler = 1;
    AddVectoredExceptionHandler(first_exception_handler, exception_handler);
}

void patch_window() {
    // Patch arguments sent to IWzGr2D::Init in CWvsApp::InitializeGr2D.
    // It's easier to patch the call site as the COM method has 15 args
    // and uses lots of VARIANTARGs :(

    // Enable windowed mode
    Patch(address::code::set_flags_for_window).write<uint32_t>(0);

    //    // Increase the window's width and height
    //    for (auto addr: {0x4A7392, 0x429A2e, 0x44A143, 0x5d711e, 0x005D6ACf, 0x5D6C4e}) {
    //        if (*reinterpret_cast<uint32_t *>(addr) == 600) {
    //            Patch(addr).write<uint32_t>(768);
    //        }
    //    }
    //    for (auto addr: {0x4A7397, 0x429A3c, 0x0044A14c, 0x5d7124, 0x5D6AD4, 0x5D6C53}) {
    //        if (*reinterpret_cast<uint32_t *>(addr) == 800) {
    //            Patch(addr).write<uint32_t>(1024);
    //        }
    //    }
}

void skip_wizet_logo() {
    Patch(0x4a6ae2)
            .nop(2)
            .skip(3)
            .nop(2)
            .skip(9)
            .nop(2);
}

void use_localhost_ips() {
    // Number of IPs (per gateway)
    Patch(0x66826C)
            .write<uint32_t>(1)
            .write<uint32_t>(1);

    // Change IPs used in static ZInetAddr initializers
    for (auto addr: {0x00668298, 0x006682A8, 0x006682B8}) {
        Patch(addr).write("127.0.0.1");
    }

    Patch(0x005010EB).write<uint32_t>(8484);
}

std::filesystem::path get_or_create_working_path() {
    char *appdata_path;
    if (_dupenv_s(&appdata_path, nullptr, "appdata")) {
        throw std::runtime_error("Failed to get %APPDATA% path");
    }

    auto path = std::filesystem::path(appdata_path) / "Maple";
    if (!std::filesystem::is_directory(path)) {
        std::filesystem::remove(path);
    }
    if (!std::filesystem::exists(path)) {
        std::filesystem::create_directory(path);
    }

    return path;
}

void disable_crypto() {
    //
    // Disable outbound crypto
    //

    // We need to zero out a4, a5, and a6:
    // seg000:0043C05E                 push    dword ptr [edi+90h] ; a6
    // seg000:0043C064                 mov     ecx, [ebp+a2]   ; this
    // seg000:0043C067                 lea     esi, [edi+90h]
    // seg000:0043C06D                 push    1               ; a5
    // seg000:0043C06F                 push    esi             ; a4
    // seg000:0043C070                 push    28h             ; a3
    // seg000:0043C072                 lea     eax, [edi+64h]
    // seg000:0043C075                 push    eax             ; a2
    // But there is no consistently-0 register at 0043C06F, and we can
    // only encode <push reg> instructions in one byte. So, we can
    // instead shift the preceding mov and lea instructions up into
    // the leftover space at 0x43c060 (down 4 bytes)

    Patch(0x43c05e)
            .push_imm(0_u8)
            .mov_reg_ireg_disp(Reg::ecx, Reg::ebp, 0x08)
            .lea_reg_ireg_disp(Reg::esi, Reg::edi, 0x90)
            .push_imm(0_u8)
            .push_imm(0_u8)
            .nop(3);

    // Disable outbound crypto on the 0x0e packet
    Patch(0x43c01e)
            .write<uint8_t>(0x6a)
            .write<uint8_t>(0x00);
    Patch(0x43c024)
            .write<uint8_t>(0x00)
            //TODO this might be fragile because ebx isn't always 0?
            .write<uint8_t>(0x53);

    //
    // Disable inbound encryption
    //

    // Don't XOR the header
    Patch(0x43c3ed).write<uint8_t>(0x00);
    // Skip sequence number checksum
    Patch(0x43c42c).nop(2);
    // Skip packet decryption
    Patch(0x4e9a97).write<uint8_t>(0xc3);
}

void enable_multi_client() {
    // The game tries to check if CreateMutexA returned error 183
    // "ERROR_ALREADY_EXISTS":
    //
    // seg000:005D5041                 call    ds:GetLastError
    // seg000:005D5047                 cmp     eax, 0B7h
    // seg000:005D504C                 jnz     loc_5D50DA

    // We can make the jump unconditional:
    Patch(0x5d504c)
            .jmp_rel(0x5d50da)
            .nop();
}

void disable_chat_restrictions() {
    // The game calls CChatHelper::TryChat to check if the player's message
    // contains a swear word (checked against a list in .WZ), if the
    // player is repeating the same message, or if the player is spamming.

    // We can make the function always return true:
    Patch(0x438ED1)
            .mov_reg_imm(Reg::eax, 1)
            .ret(4);
}

void disable_damage_cap() {
    Patch(0x0062BC80).write<double>(std::numeric_limits<double>::max());
}

void allow_cash_drops() {
    // Note that the server has to support dropping cash items as well,
    // or nothing will happen.

    // seg000:0046427A                 call    is_cash_item
    // seg000:0046427F                 test    eax, eax
    // seg000:00464281                 jnz     loc_46436A
    Patch(0x464281)
            .nop(6);

    // seg000:00464287                 mov     eax, [esi+10h]
    // seg000:0046428A                 or      eax, [esi+14h]
    // seg000:0046428D                 jnz     loc_46436A
    Patch(0x46428d)
            .nop(6);
}

void remove_gm_restrictions() {
    // Allow GMs to create parties:
    //
    // seg000:0047680F                 mov     al, [esi+0Ch]
    // seg000:00476812                 test    al, 1
    // seg000:00476814                 jz      short loc_476858
    Patch(0x476814)
            .jmp_rel_short(0x476858);

    //TODO allow GMs to join parties (string 2119)
}

void init() {
    logger = std::make_unique<Logger>();

    auto const working_path = get_or_create_working_path();
    logger->add_sink<FileLoggerSink>(working_path / "log.txt");

    auto &console = logger->add_sink<ConsoleLoggerSink>(LogLevel::Debug);
    console.set_title(fmt::format("Debug | {}", GetCurrentProcessId()));

    install_vectored_exception_handler();

    enable_multi_client();

    hook_string_pool();

    patch_window();

    skip_wizet_logo();

    use_localhost_ips();

    disable_crypto();

    install_packet_breakpoints();

    disable_chat_restrictions();

    disable_damage_cap();

    allow_cash_drops();

    remove_gm_restrictions();

    hook_d3d_create();

    logger->info("Initialized");
}