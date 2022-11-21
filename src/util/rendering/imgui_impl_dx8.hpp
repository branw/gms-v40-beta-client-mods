#ifndef MAPLE_612FDB1DF0AB40EEA97AC9C40254E2CF_HPP
#define MAPLE_612FDB1DF0AB40EEA97AC9C40254E2CF_HPP

#include <imgui.h>

struct IDirect3DDevice8;

IMGUI_IMPL_API bool     ImGui_ImplDX8_Init(IDirect3DDevice8* device);
IMGUI_IMPL_API void     ImGui_ImplDX8_Shutdown();
IMGUI_IMPL_API void     ImGui_ImplDX8_NewFrame();
IMGUI_IMPL_API void     ImGui_ImplDX8_RenderDrawData(ImDrawData* draw_data);

// Use if you want to reset your rendering device without losing Dear ImGui state.
IMGUI_IMPL_API bool     ImGui_ImplDX8_CreateDeviceObjects();
IMGUI_IMPL_API void     ImGui_ImplDX8_InvalidateDeviceObjects();

void ImGui_ImplDX8_RegisterInitialWindow();


#endif //MAPLE_612FDB1DF0AB40EEA97AC9C40254E2CF_HPP
