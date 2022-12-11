#include "rendering.hpp"

#include "cursor_icon.hpp"
#include "hooking/string_pool.hpp"
#include "util/logging/logger.hpp"
#include "util/rendering/d3d8.h"

#include <filesystem>
#include <imgui.h>
#include <imgui_stdlib.h>

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

void init_renderer(IDirect3DDevice8 *device) {
    load_cursor_texture(device);

    init_imgui();
}

void draw_cursor() {
    auto &io = ImGui::GetIO();
    auto draw_list = ImGui::GetWindowDrawList();
    draw_list->AddImage((ImTextureID)cursor_texture, io.MousePos, ImVec2(io.MousePos.x + 24, io.MousePos.y + 23));
}

void draw_menu() {
    ImGui::Checkbox("Log StringPool reads", &log_string_pool_reads);

    draw_cursor();
}

//#define FORMAT(...) fmt::format(__VA_ARGS__).c_str()
//
//struct StringPoolReplacement {
//    uint32_t index;
//    std::string original;
//    ZXString<char> replacement;
//};

//    if (TreeNode("String Pool Overrides")) {
//        PushItemWidth(250);

//        static std::vector<StringPoolReplacement> replacements{};
//        if (replacements.empty()) {
//            replacements.push_back({
//                    2133, "Ver 0.40", ZXString<char>::create("Ver 13.37")
//            });
//        }
//
//         auto const flags =
//                ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable | ImGuiTableFlags_SortMulti
//                | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_NoBordersInBody
//                | ImGuiTableFlags_ScrollY;
//
//         if (BeginTable("string_pool_overrides", 4, flags)) {
//             TableSetupColumn("ID", ImGuiTableColumnFlags_DefaultSort | ImGuiTableColumnFlags_WidthFixed);
//             TableSetupColumn("Original");
//             TableSetupColumn("Replacement");
//             TableSetupColumn("Actions", ImGuiTableColumnFlags_NoSort);
//             TableSetupScrollFreeze(0, 1);
//             TableHeadersRow();
//
//             if (auto const sort_specs = TableGetSortSpecs(); sort_specs->SpecsDirty) {
//                 std::sort(replacements.begin(), replacements.end(),
//                           [=](auto const &lhs, auto const &rhs) -> bool {
//                               for (auto i = 0; i < sort_specs->SpecsCount; i++) {
//                                   auto result = false;
//
//                                   auto const spec = sort_specs->Specs[i];
//                                   switch (spec.ColumnIndex) {
//                                   case 0: // ID
//                                       result = lhs.index < rhs.index;
//                                       break;
//
//                                   case 1: // Original
//                                       result = lhs.original.compare(rhs.original) >= 0;
//                                       break;
//
//                                   case 2: // Replacement
//                                       result = strcmp(lhs.replacement.str, rhs.replacement.str) >= 0;
//                                       break;
//                                   }
//
//                                   if (spec.SortDirection == ImGuiSortDirection_Ascending) {
//                                       result ^= true;
//                                   }
//
//                                   return result;
//                               }
//
//                               return true;
//                           });
//
//                 sort_specs->SpecsDirty = false;
//             }
//
//             ImGuiListClipper clipper;
//             clipper.Begin(static_cast<int>(replacements.size()));
//             while (clipper.Step()) {
//                 for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++) {
//                     auto &row = replacements[i];
//                     PushID(i);
//
//                     TableNextRow();
//                     TableNextColumn();
//                     Text("%4d", row.index);
//
//                     TableNextColumn();
//                     TextUnformatted(row.original.c_str());
//
//                     TableNextColumn();
//                     TextUnformatted(row.replacement.str);
//
//                     TableNextColumn();
//                     SmallButton("Foo");
//
//                     PopID();
//                 }
//             }
//
//             EndTable();
//         }

//        static uint32_t selected_index = -1;
//        if (BeginListBox("Replacements##string_pool_overrides")) {
////            for (auto &[index, replacement] : replacements) {
////                if (Selectable("foo", index == selected_index)) {
////                    selected_index = index;
////                }
////            }
//
//            Selectable("foo", true);
//            Selectable("bar", false);
//
//            EndListBox();
//        }
//        SameLine();
//
//        BeginGroup();
//        static std::string replacement{};
//        InputText("Replacement", &replacement);
//
//        EndGroup();

//        PopItemWidth();
//        TreePop();
//    }
