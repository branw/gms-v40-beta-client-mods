#include <Windows.h>
#include <imgui_impl_win32.h>

#include "graphics.hpp"
#include "../util/logging/logger.hpp"
#include "../util/hooking/export.hpp"
#include "../util/rendering/d3d8.h"
#include "../util/rendering/imgui_impl_dx8.hpp"
#include "../rendering.hpp"

void hook_end_scene(IDirect3DDevice8 *device) {
    using EndScene_type = HRESULT (WINAPI*)(LPDIRECT3DDEVICE8);
    static EndScene_type EndScene_original{};
    static EndScene_type EndScene_hook = [](LPDIRECT3DDEVICE8 device) -> HRESULT {
        ImGui_ImplDX8_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetMouseCursor(ImGuiMouseCursor_None);

        ImGui::Begin("Maple");
        draw_menu();
        ImGui::End();

        ImGui::Render();
        ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());

        auto &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        return EndScene_original(device);
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

        if (uMsg == WM_PAINT) {
            ValidateRect(hWnd, nullptr);
        }

        // Avoid passing events to the game when interacting with the menu,
        // except for messages with uMsg=1025 used by the game engine
        auto const &io = ImGui::GetIO();
        if ((io.WantCaptureMouse|| io.WantCaptureKeyboard) && uMsg < WM_USER) {
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
        auto const result = CreateDevice_original(Direct3D_Object, Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
        if (result != D3D_OK) {
            logger->error("CreateDevice failed: {:x}", result);
            return result;
        }

        auto const device = *ppReturnedDeviceInterface;

        ImGui::CreateContext();

        init_renderer(device);

        ImGui_ImplWin32_Init(hFocusWindow);
        ImGui_ImplDX8_Init(device);

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
        if (SDKVersion != D3D_SDK_VERSION) {
            logger->error("D3D SDK version {} does not match D3D_SDK_VERSION {}", SDKVersion, D3D_SDK_VERSION);
        }

        auto const d3d = Direct3DCreate8_original(SDKVersion);
        hook_create_device(d3d);
        return d3d;
    };

    if (hook_export("d3d8.dll", "Direct3DCreate8", &Direct3DCreate8_original, Direct3DCreate8_hook)) {
        logger->info("Direct3DCreate8 hook installed");
    }
}