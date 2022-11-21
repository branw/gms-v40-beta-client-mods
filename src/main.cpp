#define NOMINMAX

#include "cursor_icon.hpp"
#include "dinput8.hpp"
#include "hooking/bootstrap.hpp"
#include "hooking/packets.hpp"
#include "hooking/string_pool.hpp"
#include "maple/addresses.hpp"
#include "util/hooking/export.hpp"
#include "util/logging/console_logger_sink.hpp"
#include "util/logging/file_logger_sink.hpp"
#include "util/logging/stack_trace.hpp"
#include "util/memory/patch.hpp"
#include "util/rendering/d3d8.h"
#include "util/rendering/imgui_impl_dx8.hpp"

#include <Windows.h>
#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <imgui_impl_win32.h>
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

LONG CALLBACK packet_hook(PEXCEPTION_POINTERS ex) {
    auto const addr = reinterpret_cast<uintptr_t>(ex->ExceptionRecord->ExceptionAddress);
    auto const ctx = ex->ContextRecord;

    if (
            handle_string_pool_hooks(addr, ctx) ||
            handle_out_packet_hooks(addr, ctx) ||
            handle_in_packet_hooks(addr, ctx)) {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
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

void init_imgui() {
    auto &style = ImGui::GetStyle();
    style.WindowPadding = {10.f, 10.f};
    style.PopupRounding = 0.f;
    style.FramePadding = {8.f, 4.f};
    style.ItemSpacing = {10.f, 8.f};
    style.ItemInnerSpacing = {6.f, 6.f};
    style.TouchExtraPadding = {0.f, 0.f};
    style.IndentSpacing = 21.f;
    style.ScrollbarSize = 15.f;
    style.GrabMinSize = 8.f;
    style.WindowBorderSize = 1.f;
    style.ChildBorderSize = 0.f;
    style.PopupBorderSize = 1.f;
    style.FrameBorderSize = 0.f;
    style.TabBorderSize = 0.f;
    style.WindowRounding = 0.f;
    style.ChildRounding = 0.f;
    style.FrameRounding = 0.f;
    style.ScrollbarRounding = 0.f;
    style.GrabRounding = 0.f;
    style.TabRounding = 0.f;
    style.WindowTitleAlign = {0.5f, 0.5f};
    style.ButtonTextAlign = {0.5f, 0.5f};
    style.DisplaySafeAreaPadding = {3.f, 3.f};

    auto &colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(1.00f, 0.90f, 0.19f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border] = ImVec4(0.30f, 0.30f, 0.30f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.21f, 0.21f, 0.21f, 0.78f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.27f, 0.27f, 0.54f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.39f, 0.38f, 0.38f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.41f, 0.41f, 0.41f, 0.74f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.41f, 0.41f, 0.41f, 0.78f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.87f);
    colors[ImGuiCol_Header] = ImVec4(0.37f, 0.37f, 0.37f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.38f, 0.38f, 0.38f, 0.37f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.37f, 0.37f, 0.37f, 0.51f);
    colors[ImGuiCol_Separator] = ImVec4(0.38f, 0.38f, 0.38f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.46f, 0.46f, 0.46f, 0.50f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.64f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.21f, 0.21f, 0.21f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.27f, 0.27f, 0.27f, 0.86f);
    colors[ImGuiCol_TabActive] = ImVec4(0.34f, 0.34f, 0.34f, 0.86f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.10f, 0.10f, 0.10f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

    auto &io = ImGui::GetIO();

    // Allow ImGui windows to be dragged outside the game window
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Disable ImGui config
    io.IniFilename = nullptr;

    // Change to our own font, hopefully on the system
    auto const font_path = R"(C:\Windows\Fonts\Bahnschrift.ttf)";
    if (std::filesystem::exists(font_path)) {
        io.Fonts->AddFontFromFileTTF(font_path, 14);
    } else {
        logger->warning("Could not load font");
    }
}

IDirect3DTexture8 *cursor_texture{};

void load_cursor_texture(IDirect3DDevice8 *device) {
    auto hr = device->CreateTexture(24, 23, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &cursor_texture);
    if (FAILED(hr)) {
        logger->error("CreateTexture {:x}", hr);
    }

    IDirect3DSurface8 *work_surface{};
    cursor_texture->GetSurfaceLevel(0, &work_surface);

    D3DLOCKED_RECT work_rect{0};
    work_surface->LockRect(&work_rect, nullptr, 0);

    auto input = cursor_bitmap.data();
    auto output = reinterpret_cast<BYTE *>(work_rect.pBits);
    auto const texture_pitch = work_rect.Pitch;

    for (auto y = 0; y < cursor_height; y++) {
        memcpy(output, input, cursor_width * 4);
        output += texture_pitch;
        input += cursor_width * 4;
    }

    work_surface->UnlockRect();
    work_surface->Release();
}

void hook_end_scene(IDirect3DDevice8 *device) {
    using EndScene_type = HRESULT (WINAPI*)(LPDIRECT3DDEVICE8);
    static EndScene_type EndScene_original{};
    static EndScene_type EndScene_hook = [](LPDIRECT3DDEVICE8 device) -> HRESULT {
        auto const result = EndScene_original(device);

        ImGui_ImplDX8_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        ImGui::Begin("Hello, world!");
        bool b;
        ImGui::Checkbox("111", &b);

        auto &io = ImGui::GetIO();

        //TODO fix cursor
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        draw_list->AddImage((ImTextureID)cursor_texture, io.MousePos, ImVec2(io.MousePos.x + 24, io.MousePos.y + 23));

        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        return result;
    };

    auto const vtable = *reinterpret_cast<uintptr_t **>(device);
    auto const EndScene_index = 35;

    DWORD old_protect{};
    if (!VirtualProtect(&vtable[EndScene_index], sizeof(uintptr_t), PAGE_READWRITE, &old_protect)) {
        logger->error("VP1 EndScene failed");
        return;
    }

    EndScene_original = reinterpret_cast<EndScene_type>(vtable[EndScene_index]);
    vtable[EndScene_index] = reinterpret_cast<uintptr_t>(+EndScene_hook);

    if (!VirtualProtect(&vtable[EndScene_index], sizeof(uintptr_t), old_protect, &old_protect)) {
        logger->error("VP2 EndScene failed");
        return;
    }

    logger->debug("EndScene hook installed");
}

void hook_wndproc(HWND window) {
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    static WNDPROC WndProc_original;
    static WNDPROC WndProc_hook = [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) -> LRESULT {
        ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

        auto const &io = ImGui::GetIO();
        if (io.WantCaptureMouse|| io.WantCaptureKeyboard) {
            return TRUE;
        }

        return CallWindowProcW(WndProc_original, hWnd, uMsg, wParam, lParam);
    };

    WndProc_original = reinterpret_cast<WNDPROC>(SetWindowLongPtrW(
            window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(+WndProc_hook)));

    logger->debug("WndProc hook installed");
}

void hook_create_device(IDirect3D8 *d3d) {
    using CreateDevice_type = HRESULT (__stdcall *)(IDirect3D8* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface);
    static CreateDevice_type CreateDevice_original{};
    static CreateDevice_type CreateDevice_hook = [](IDirect3D8* Direct3D_Object, UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface) -> HRESULT {
        auto result = CreateDevice_original(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        if (result != D3D_OK) {
            logger->error("CreateDevice failed: {:x}", result);
            return result;
        }

        auto const device = *ppReturnedDeviceInterface;

        ImGui::CreateContext();
        init_imgui();
        ImGui_ImplWin32_Init(hFocusWindow);
        ImGui_ImplDX8_Init(device);

        load_cursor_texture(device);

        logger->debug("GUI initialized");

        hook_end_scene(device);
        hook_wndproc(hFocusWindow);

        return result;
    };

    auto const vtable = *reinterpret_cast<uintptr_t **>(d3d);
    auto const CreateDevice_index = 15;

    DWORD old_protect{};
    if (!VirtualProtect(&vtable[CreateDevice_index], sizeof(uintptr_t), PAGE_READWRITE, &old_protect)) {
        logger->error("VP1 CreateDevice failed");
        return;
    }

    CreateDevice_original = reinterpret_cast<CreateDevice_type>(vtable[CreateDevice_index]);
    vtable[CreateDevice_index] = reinterpret_cast<uintptr_t>(+CreateDevice_hook);

    if (!VirtualProtect(&vtable[CreateDevice_index], sizeof(uintptr_t), old_protect, &old_protect)) {
        logger->error("VP2 CreateDevice failed");
        return;
    }

    logger->debug("CreateDevice hook installed");
}

void hook_d3d_create() {
    using Direct3DCreate8_type = IDirect3D8 *(__stdcall *)(UINT);
    static Direct3DCreate8_type Direct3DCreate8_original{};
    static Direct3DCreate8_type Direct3DCreate8_hook = [](UINT SDKVersion) -> IDirect3D8 * {
        auto const d3d = Direct3DCreate8_original(SDKVersion);

        hook_create_device(d3d);

        return d3d;
    };

    if (hook_export("d3d8.dll", "Direct3DCreate8", &Direct3DCreate8_original, Direct3DCreate8_hook)) {
        logger->info("Direct3DCreate8 hook installed");
    }
}

void init() {
    logger = std::make_unique<Logger>();

    auto const working_path = get_or_create_working_path();
    logger->add_sink<FileLoggerSink>(working_path / "log.txt");

    auto &console = logger->add_sink<ConsoleLoggerSink>(LogLevel::Debug);
    console.set_title(fmt::format("Debug | {}", GetCurrentProcessId()));

    install_exception_logger();

    enable_multi_client();

    hook_string_pool();

    patch_window();

    skip_wizet_logo();

    use_localhost_ips();

    disable_crypto();

    uint32_t const first_exception_handler = 1;
    AddVectoredExceptionHandler(first_exception_handler, packet_hook);
    install_packet_breakpoints();

    disable_chat_restrictions();

    disable_damage_cap();

    allow_cash_drops();

    remove_gm_restrictions();

    hook_d3d_create();

    logger->info("Initialized");
}