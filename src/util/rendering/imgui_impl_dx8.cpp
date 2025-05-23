#include "imgui_impl_dx8.hpp"

#include <imgui.h>

// DirectX
#include "d3d8.h"

// DirectX data
struct ImGui_ImplDX8_Data {
    LPDIRECT3DDEVICE8           pd3dDevice{};
    LPDIRECT3DVERTEXBUFFER8     pVB{};
    LPDIRECT3DINDEXBUFFER8      pIB{};
    LPDIRECT3DVERTEXBUFFER8     maskVB{};
    LPDIRECT3DINDEXBUFFER8      maskIB{};
    LPDIRECT3DTEXTURE8          FontTexture{};
    int                         VertexBufferSize;
    int                         IndexBufferSize;

    ImGui_ImplDX8_Data() {
        memset(this, 0, sizeof(*this));
        VertexBufferSize = 5000;
        IndexBufferSize = 10000;
    }
};

struct CUSTOMVERTEX {
    float    pos[3];
    D3DCOLOR col;
    float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

#ifdef IMGUI_USE_BGRA_PACKED_COLOR
#define IMGUI_COL_TO_DX8_ARGB(_COL)     (_COL)
#else
#define IMGUI_COL_TO_DX8_ARGB(_COL)     (((_COL) & 0xFF00FF00) | (((_COL) & 0xFF0000) >> 16) | (((_COL) & 0xFF) << 16))
#endif

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplDX8_Data *ImGui_ImplDX8_GetBackendData() {
    if (!ImGui::GetCurrentContext()) {
        return nullptr;
    }
    return static_cast<ImGui_ImplDX8_Data *>(ImGui::GetIO().BackendRendererUserData);
}

// Forward Declarations
static void ImGui_ImplDX8_InitPlatformInterface();
static void ImGui_ImplDX8_ShutdownPlatformInterface();
static void ImGui_ImplDX8_CreateDeviceObjectsForPlatformWindows();
static void ImGui_ImplDX8_InvalidateDeviceObjectsForPlatformWindows();

// Functions
static void ImGui_ImplDX8_SetupRenderState(ImDrawData const * draw_data) {
    auto * const bd = ImGui_ImplDX8_GetBackendData();

    // Setup viewport
    D3DVIEWPORT8 vp;
    vp.X = vp.Y = 0;
    vp.Width = static_cast<DWORD>(draw_data->DisplaySize.x);
    vp.Height = static_cast<DWORD>(draw_data->DisplaySize.y);
    vp.MinZ = 0.0f;
    vp.MaxZ = 1.0f;
    bd->pd3dDevice->SetViewport(&vp);

    // Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing, shade mode (for gradient)
    bd->pd3dDevice->SetPixelShader(NULL);
    bd->pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);
    bd->pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    bd->pd3dDevice->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
    bd->pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
    bd->pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    bd->pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    bd->pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    bd->pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    bd->pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);
    bd->pd3dDevice->SetRenderState(D3DRS_CLIPPING, TRUE);
    bd->pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    bd->pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
    bd->pd3dDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    bd->pd3dDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

    // Setup orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    // Being agnostic of whether <d3dx8.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
    {
        float L = draw_data->DisplayPos.x + 0.5f;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x + 0.5f;
        float T = draw_data->DisplayPos.y + 0.5f;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y + 0.5f;
        D3DMATRIX mat_identity = { { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } } };
        D3DMATRIX mat_projection =
                { { {
                        2.0f/(R-L),   0.0f,         0.0f,  0.0f,
                        0.0f,         2.0f/(T-B),   0.0f,  0.0f,
                        0.0f,         0.0f,         0.5f,  0.0f,
                        (L+R)/(L-R),  (T+B)/(B-T),  0.5f,  1.0f
                } } };
        bd->pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
        bd->pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
        bd->pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
    }
}

void build_mask_vbuffer(const RECT* rect) {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    CUSTOMVERTEX* vtx_dst{};
    bd->maskVB->Lock(0, 6 * sizeof(CUSTOMVERTEX), reinterpret_cast<BYTE **>(&vtx_dst), 0);
    vtx_dst[0].pos[0] = static_cast<float>(rect->left);
    vtx_dst[0].pos[1] = static_cast<float>(rect->bottom);
    vtx_dst[0].pos[2] = 0;
    vtx_dst[1].pos[0] = static_cast<float>(rect->left);
    vtx_dst[1].pos[1] = static_cast<float>(rect->top);
    vtx_dst[1].pos[2] = 0;
    vtx_dst[2].pos[0] = static_cast<float>(rect->right);
    vtx_dst[2].pos[1] = static_cast<float>(rect->top);
    vtx_dst[2].pos[2] = 0;
    vtx_dst[3].pos[0] = static_cast<float>(rect->left);
    vtx_dst[3].pos[1] = static_cast<float>(rect->bottom);
    vtx_dst[3].pos[2] = 0;
    vtx_dst[4].pos[0] = static_cast<float>(rect->right);
    vtx_dst[4].pos[1] = static_cast<float>(rect->top);
    vtx_dst[4].pos[2] = 0;
    vtx_dst[5].pos[0] = static_cast<float>(rect->right);
    vtx_dst[5].pos[1] = static_cast<float>(rect->bottom);
    vtx_dst[5].pos[2] = 0;
    vtx_dst[0].col = 0xFFFFFFFF;
    vtx_dst[1].col = 0xFFFFFFFF;
    vtx_dst[2].col = 0xFFFFFFFF;
    vtx_dst[3].col = 0xFFFFFFFF;
    vtx_dst[4].col = 0xFFFFFFFF;
    vtx_dst[5].col = 0xFFFFFFFF;
    vtx_dst[0].uv[0] = 0;
    vtx_dst[0].uv[1] = 0;
    vtx_dst[1].uv[0] = 0;
    vtx_dst[1].uv[1] = 0;
    vtx_dst[2].uv[0] = 0;
    vtx_dst[2].uv[1] = 0;
    vtx_dst[3].uv[0] = 0;
    vtx_dst[3].uv[1] = 0;
    vtx_dst[4].uv[0] = 0;
    vtx_dst[4].uv[1] = 0;
    vtx_dst[5].uv[0] = 0;
    vtx_dst[5].uv[1] = 0;
    bd->maskVB->Unlock();
}

// Render function.
void ImGui_ImplDX8_RenderDrawData(ImDrawData* draw_data) {
    // Avoid rendering when minimized
    if (draw_data->DisplaySize.x <= 0.0f || draw_data->DisplaySize.y <= 0.0f) {
        return;
    }

    // Create and grow buffers if needed
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    if (!bd->pVB || bd->VertexBufferSize < draw_data->TotalVtxCount) {
        if (bd->pVB) {
            bd->pVB->Release();
            bd->pVB = nullptr;
        }
        bd->VertexBufferSize = draw_data->TotalVtxCount + 5000;
        if (bd->pd3dDevice->CreateVertexBuffer(
                bd->VertexBufferSize * sizeof(CUSTOMVERTEX),
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                D3DFVF_CUSTOMVERTEX,
                D3DPOOL_DEFAULT,
                &bd->pVB) < 0) {
            return;
        }
    }
    if (!bd->pIB || bd->IndexBufferSize < draw_data->TotalIdxCount) {
        if (bd->pIB) {
            bd->pIB->Release();
            bd->pIB = nullptr;
        }
        bd->IndexBufferSize = draw_data->TotalIdxCount + 10000;
        if (bd->pd3dDevice->CreateIndexBuffer(
                bd->IndexBufferSize * sizeof(ImDrawIdx),
                D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
                sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32,
             D3DPOOL_DEFAULT, &bd->pIB) < 0) {
            return;
        }
    }

    if (!bd->maskVB && !bd->maskIB) {
        if (bd->pd3dDevice->CreateVertexBuffer(6 * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &bd->maskVB) < 0) {
            return;
        }
        if (bd->pd3dDevice->CreateIndexBuffer(6, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &bd->maskIB) < 0) {
            return;
        }

        ImDrawIdx* idx_dst{};
        bd->maskIB->Lock(0, 6 * sizeof(ImDrawIdx), reinterpret_cast<BYTE **>(&idx_dst), D3DLOCK_DISCARD);
        idx_dst[0] = 0;
        idx_dst[1] = 1;
        idx_dst[2] = 2;
        idx_dst[3] = 0;
        idx_dst[4] = 2;
        idx_dst[5] = 3;
        bd->maskIB->Unlock();
    }

    // Backup the DX8 pixel and vertex state
    DWORD state_block_token{};
    if (bd->pd3dDevice->CreateStateBlock(D3DSBT_ALL, &state_block_token) < 0) {
        return;
    }
    if (bd->pd3dDevice->CaptureStateBlock(state_block_token) < 0) {
        bd->pd3dDevice->DeleteStateBlock(state_block_token);
        return;
    }

    // Backup the DX8 transform
    D3DMATRIX last_world, last_view, last_projection;
    bd->pd3dDevice->GetTransform(D3DTS_WORLD, &last_world);
    bd->pd3dDevice->GetTransform(D3DTS_VIEW, &last_view);
    bd->pd3dDevice->GetTransform(D3DTS_PROJECTION, &last_projection);

    // Allocate buffers
    CUSTOMVERTEX* vtx_dst;
    ImDrawIdx* idx_dst;
    if (bd->pVB->Lock(0, draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX), reinterpret_cast<BYTE **>(&vtx_dst), D3DLOCK_DISCARD) < 0) {
        bd->pd3dDevice->DeleteStateBlock(state_block_token);
        return;
    }
    if (bd->pIB->Lock(0, draw_data->TotalIdxCount * sizeof(ImDrawIdx), reinterpret_cast<BYTE **>(&idx_dst), D3DLOCK_DISCARD) < 0) {
        bd->pVB->Unlock();
        bd->pd3dDevice->DeleteStateBlock(state_block_token);
        return;
    }

    // Copy and convert all vertices into a single contiguous buffer, convert colors to DX8 default format.
    // FIXME-OPT: This is a minor waste of resource, the ideal is to use imconfig.h and
    //  1) to avoid repacking colors:   #define IMGUI_USE_BGRA_PACKED_COLOR
    //  2) to avoid repacking vertices: #define IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT struct ImDrawVert { ImVec2 pos; float z; ImU32 col; ImVec2 uv; }
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        ImDrawList const * cmd_list = draw_data->CmdLists[n];
        ImDrawVert const * vtx_src = cmd_list->VtxBuffer.Data;
        for (int i = 0; i < cmd_list->VtxBuffer.Size; i++) {
            vtx_dst->pos[0] = vtx_src->pos.x;
            vtx_dst->pos[1] = vtx_src->pos.y;
            vtx_dst->pos[2] = 0.0f;
            vtx_dst->col = IMGUI_COL_TO_DX8_ARGB(vtx_src->col);
            vtx_dst->uv[0] = vtx_src->uv.x;
            vtx_dst->uv[1] = vtx_src->uv.y;
            vtx_dst++;
            vtx_src++;
        }
        memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        idx_dst += cmd_list->IdxBuffer.Size;
    }
    bd->pVB->Unlock();
    bd->pIB->Unlock();
    bd->pd3dDevice->SetStreamSource(0, bd->pVB, sizeof(CUSTOMVERTEX));
    bd->pd3dDevice->SetIndices(bd->pIB, 0);
    bd->pd3dDevice->SetVertexShader(D3DFVF_CUSTOMVERTEX);

    // Setup desired DX state
    ImGui_ImplDX8_SetupRenderState(draw_data);

    // Render command lists
    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int global_vtx_offset = 0;
    int global_idx_offset = 0;
    ImVec2 const clip_off = draw_data->DisplayPos;
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        ImDrawList const * cmd_list = draw_data->CmdLists[n];
        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
            ImDrawCmd const * pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
                    ImGui_ImplDX8_SetupRenderState(draw_data);
                } else {
                    pcmd->UserCallback(cmd_list, pcmd);
                }
            } else {
                // Project clipping rectangles into framebuffer space
                ImVec2 const clip_min(pcmd->ClipRect.x - clip_off.x, pcmd->ClipRect.y - clip_off.y);
                ImVec2 const clip_max(pcmd->ClipRect.z - clip_off.x, pcmd->ClipRect.w - clip_off.y);
                if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                    continue;

                // Apply clipping rectangle, Bind texture, Draw
                const RECT r = {static_cast<LONG>(clip_min.x), static_cast<LONG>(clip_min.y), static_cast<LONG>(clip_max.x), static_cast<LONG>(clip_max.y)};
                auto const texture = static_cast<LPDIRECT3DTEXTURE8>(pcmd->GetTexID());
                bd->pd3dDevice->SetTexture(0, texture);
                build_mask_vbuffer(&r);
                bd->pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
                bd->pd3dDevice->SetRenderState(D3DRS_ZENABLE, true);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0xFF);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFF);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
                bd->pd3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0xFF);
                bd->pd3dDevice->Clear(0, nullptr, D3DCLEAR_STENCIL, 0, 1.0f, 0);
                bd->pd3dDevice->SetStreamSource(0, bd->maskVB, sizeof(CUSTOMVERTEX));
                bd->pd3dDevice->SetIndices(bd->maskIB, 0);
                bd->pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 4, 0, 2);
                bd->pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
                bd->pd3dDevice->SetStreamSource(0, bd->pVB, sizeof(CUSTOMVERTEX));
                bd->pd3dDevice->SetIndices(bd->pIB, global_vtx_offset);
                bd->pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xF);
                bd->pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILENABLE, true);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILWRITEMASK, 0);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILMASK, 0xFF);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
                bd->pd3dDevice->SetRenderState(D3DRS_STENCILREF, 0xFF);
                bd->pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, static_cast<UINT>(cmd_list->VtxBuffer.Size), pcmd->IdxOffset + global_idx_offset, pcmd->ElemCount / 3);
            }
        }
        global_idx_offset += cmd_list->IdxBuffer.Size;
        global_vtx_offset += cmd_list->VtxBuffer.Size;
    }

    // Restore the DX8 transform
    bd->pd3dDevice->SetTransform(D3DTS_WORLD, &last_world);
    bd->pd3dDevice->SetTransform(D3DTS_VIEW, &last_view);
    bd->pd3dDevice->SetTransform(D3DTS_PROJECTION, &last_projection);

    // Restore the DX8 state
    bd->pd3dDevice->ApplyStateBlock(state_block_token);
    bd->pd3dDevice->DeleteStateBlock(state_block_token);
}


bool ImGui_ImplDX8_Init(IDirect3DDevice8* device) {
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    auto * const bd = IM_NEW(ImGui_ImplDX8_Data)();
    io.BackendRendererUserData = static_cast<void *>(bd);
    io.BackendRendererName = "imgui_impl_DX8";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field, allowing for large meshes.
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    bd->pd3dDevice = device;
    bd->pd3dDevice->AddRef();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        ImGui_ImplDX8_InitPlatformInterface();
    }

    return true;
}

void ImGui_ImplDX8_Shutdown() {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplDX8_ShutdownPlatformInterface();
    ImGui_ImplDX8_InvalidateDeviceObjects();
    if (bd->pd3dDevice) {
        bd->pd3dDevice->Release();
    }
    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    IM_DELETE(bd);
}

static bool ImGui_ImplDX8_CreateFontsTexture() {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    ImGuiIO const & io = ImGui::GetIO();

    // Build texture atlas
    unsigned char* pixels;
    int width, height, bytes_per_pixel;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

    // Convert RGBA32 to BGRA32 (because RGBA32 is not well-supported by DX8 devices)
#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors) {
        auto* dst_start = static_cast<ImU32 *>(ImGui::MemAlloc(static_cast<size_t>(width) * height * bytes_per_pixel));
        for (ImU32* src = reinterpret_cast<ImU32 *>(pixels), *dst = dst_start, *dst_end = dst_start + static_cast<size_t>(width) * height; dst < dst_end; src++, dst++) {
            *dst = IMGUI_COL_TO_DX8_ARGB(*src);
        }
        pixels = reinterpret_cast<unsigned char *>(dst_start);
    }
#endif

    // Upload texture to graphics system
    bd->FontTexture = nullptr;
    if (bd->pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &bd->FontTexture) < 0) {
        return false;
    }
    D3DLOCKED_RECT tex_locked_rect;
    if (bd->FontTexture->LockRect(0, &tex_locked_rect, nullptr, 0) != D3D_OK) {
        return false;
    }
    for (int y = 0; y < height; y++) {
        memcpy(
            static_cast<unsigned char *>(tex_locked_rect.pBits) + static_cast<size_t>(tex_locked_rect.Pitch) * y,
            pixels + static_cast<size_t>(width) * bytes_per_pixel * y,
            static_cast<size_t>(width) * bytes_per_pixel);
    }
    bd->FontTexture->UnlockRect(0);

    // Store our identifier
    io.Fonts->SetTexID(bd->FontTexture);

#ifndef IMGUI_USE_BGRA_PACKED_COLOR
    if (io.Fonts->TexPixelsUseColors) {
        ImGui::MemFree(pixels);
    }
#endif

    return true;
}

bool ImGui_ImplDX8_CreateDeviceObjects() {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    if (!bd || !bd->pd3dDevice) {
        return false;
    }
    if (!ImGui_ImplDX8_CreateFontsTexture()) {
        return false;
    }
    ImGui_ImplDX8_CreateDeviceObjectsForPlatformWindows();
    return true;
}

void ImGui_ImplDX8_InvalidateDeviceObjects() {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    if (!bd || !bd->pd3dDevice) {
        return;
    }
    if (bd->pVB) {
        bd->pVB->Release();
        bd->pVB = nullptr;
    }
    if (bd->pIB) {
        bd->pIB->Release();
        bd->pIB = nullptr;
    }
    if (bd->maskVB) {
        bd->maskVB->Release();
        bd->maskVB = nullptr;
    }
    if (bd->maskIB) {
        bd->maskIB->Release();
        bd->maskIB = nullptr;
    }
    if (bd->FontTexture) {
        bd->FontTexture->Release();
        bd->FontTexture = nullptr;
        // We copied bd->pFontTextureView to io.Fonts->TexID so let's clear that as well.
        ImGui::GetIO().Fonts->SetTexID(nullptr);
    }
    ImGui_ImplDX8_InvalidateDeviceObjectsForPlatformWindows();
}

void ImGui_ImplDX8_NewFrame() {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    IM_ASSERT(bd != nullptr && "Did you call ImGui_ImplDX8_Init()?");

    if (!bd->FontTexture) {
        ImGui_ImplDX8_CreateDeviceObjects();
    }
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGui_ImplDX8_ViewportData {
    IDirect3DSwapChain8*    SwapChain;
    D3DPRESENT_PARAMETERS   d3dpp{};

    ImGui_ImplDX8_ViewportData() {
        SwapChain = nullptr;
        ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    }
    ~ImGui_ImplDX8_ViewportData() {
        IM_ASSERT(SwapChain == nullptr);
    }
};

static void ImGui_ImplDX8_CreateWindow(ImGuiViewport* viewport) {
    ImGui_ImplDX8_Data *bd = ImGui_ImplDX8_GetBackendData();
    auto *vd = IM_NEW(ImGui_ImplDX8_ViewportData)();
    viewport->RendererUserData = vd;

    // PlatformHandleRaw should always be a HWND, whereas PlatformHandle might be a higher-level handle (e.g. GLFWWindow*, SDL_Window*).
    // Some backends will leave PlatformHandleRaw NULL, in which case we assume PlatformHandle will contain the HWND.
    HWND hwnd = viewport->PlatformHandleRaw ? static_cast<HWND>(viewport->PlatformHandleRaw) : static_cast<HWND>(viewport->PlatformHandle);
    IM_ASSERT(hwnd != 0);

    ZeroMemory(&vd->d3dpp, sizeof(D3DPRESENT_PARAMETERS));
    vd->d3dpp.Windowed = TRUE;
    vd->d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    vd->d3dpp.BackBufferWidth = static_cast<UINT>(viewport->Size.x);
    vd->d3dpp.BackBufferHeight = static_cast<UINT>(viewport->Size.y);
    vd->d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8; //D3DFMT_UNKNOWN;
    vd->d3dpp.BackBufferCount = 1;
    vd->d3dpp.hDeviceWindow = hwnd;
    vd->d3dpp.EnableAutoDepthStencil = FALSE;
    vd->d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

    HRESULT hr = bd->pd3dDevice->CreateAdditionalSwapChain(&vd->d3dpp, &vd->SwapChain); IM_UNUSED(hr);
    IM_ASSERT(hr == D3D_OK);
    IM_ASSERT(vd->SwapChain != nullptr);
}

static void ImGui_ImplDX8_DestroyWindow(ImGuiViewport* viewport) {
    // The main viewport (owned by the application) will always have RendererUserData == NULL since we didn't create the data for it.
    if (auto* vd = static_cast<ImGui_ImplDX8_ViewportData *>(viewport->RendererUserData)) {
        if (vd->SwapChain)
            vd->SwapChain->Release();
        vd->SwapChain = nullptr;
        ZeroMemory(&vd->d3dpp, sizeof(D3DPRESENT_PARAMETERS));
        IM_DELETE(vd);
    }
    viewport->RendererUserData = nullptr;
}

static void ImGui_ImplDX8_SetWindowSize(ImGuiViewport* viewport, ImVec2 size) {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    auto * const vd = static_cast<ImGui_ImplDX8_ViewportData *>(viewport->RendererUserData);
    if (vd->SwapChain) {
        vd->SwapChain->Release();
        vd->SwapChain = nullptr;
        vd->d3dpp.BackBufferWidth = static_cast<UINT>(size.x);
        vd->d3dpp.BackBufferHeight = static_cast<UINT>(size.y);
        HRESULT hr = bd->pd3dDevice->CreateAdditionalSwapChain(&vd->d3dpp, &vd->SwapChain); IM_UNUSED(hr);
        IM_ASSERT(hr == D3D_OK);
    }
}

static ImVec2 ImGui_ImplDX8_GetWindowPos(ImGuiViewport* viewport) {
    //TODO
    return { 0, 0 };
}


static void ImGui_ImplDX8_RenderWindow(ImGuiViewport* viewport, void*) {
    auto * const bd = ImGui_ImplDX8_GetBackendData();
    auto * const vd = static_cast<ImGui_ImplDX8_ViewportData *>(viewport->RendererUserData);
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    LPDIRECT3DSURFACE8 render_target = nullptr;
    LPDIRECT3DSURFACE8 last_render_target = nullptr;
    LPDIRECT3DSURFACE8 last_depth_stencil = nullptr;
    vd->SwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &render_target);
    bd->pd3dDevice->GetRenderTarget(&last_render_target);
    bd->pd3dDevice->GetDepthStencilSurface(&last_depth_stencil);
    bd->pd3dDevice->SetRenderTarget(render_target, nullptr);

    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA(static_cast<int>(clear_color.x * 255.0f), static_cast<int>(clear_color.y * 255.0f), static_cast<int>(clear_color.z * 255.0f), static_cast<int>(clear_color.w * 255.0f));
        bd->pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET, clear_col_dx, 1.0f, 0);
    }

    ImGui_ImplDX8_RenderDrawData(viewport->DrawData);

    // Restore render target
    bd->pd3dDevice->SetRenderTarget(last_render_target, nullptr);
    render_target->Release();
    last_render_target->Release();
    if (last_depth_stencil) {
        last_depth_stencil->Release();
    }
}

static void ImGui_ImplDX8_SwapBuffers(ImGuiViewport* viewport, void*) {
    auto * const vd = static_cast<ImGui_ImplDX8_ViewportData *>(viewport->RendererUserData);
    HRESULT hr = vd->SwapChain->Present(nullptr, nullptr, vd->d3dpp.hDeviceWindow, nullptr);
    // Let main application handle D3DERR_DEVICELOST by resetting the device.
    IM_ASSERT(hr == D3D_OK || hr == D3DERR_DEVICELOST);
}

static void ImGui_ImplDX8_InitPlatformInterface() {
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    platform_io.Renderer_CreateWindow = ImGui_ImplDX8_CreateWindow;
    platform_io.Renderer_DestroyWindow = ImGui_ImplDX8_DestroyWindow;
    platform_io.Renderer_SetWindowSize = ImGui_ImplDX8_SetWindowSize;
    platform_io.Renderer_RenderWindow = ImGui_ImplDX8_RenderWindow;
    platform_io.Renderer_SwapBuffers = ImGui_ImplDX8_SwapBuffers;
}

static void ImGui_ImplDX8_ShutdownPlatformInterface() {
    ImGui::DestroyPlatformWindows();
}

static void ImGui_ImplDX8_CreateDeviceObjectsForPlatformWindows() {
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++) {
        if (!platform_io.Viewports[i]->RendererUserData) {
            ImGui_ImplDX8_CreateWindow(platform_io.Viewports[i]);
        }
    }
}

static void ImGui_ImplDX8_InvalidateDeviceObjectsForPlatformWindows() {
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int i = 1; i < platform_io.Viewports.Size; i++) {
        if (platform_io.Viewports[i]->RendererUserData) {
            ImGui_ImplDX8_DestroyWindow(platform_io.Viewports[i]);
        }
    }
}
