#ifndef MAPLE_2842A2F3CC18409CAE9C402C69931325_HPP
#define MAPLE_2842A2F3CC18409CAE9C402C69931325_HPP

#include <cstdint>

namespace address {
    namespace func {
        uint32_t const WinMain = 0x5d4fc0;
    }

    namespace code {
        uint32_t const set_flags_for_window = 0x5d70b1;
    }

    namespace vfunc {
        //
        // IWzGr2D: e576ea33_d465_4f08_aab1_e78df73ee6d9
        //
        auto const IWzGr2D_Init = 3;// guessed, inlined in CWvsApp::InitializeGr2D
        auto const IWzGr2D_GetnextRenderTime = 5;
        auto const IWzGr2D_UpdateFrame = 6;// guessed, inlined in CWvsApp::CallUpdate
        auto const IWzGr2D_RenderFrame = 7;
        auto const IWzGr2D_SetFrameSkip = 8;
        auto const IWzGr2D_ToggleFpsPanel = 9;
        auto const IWzGr2D_Getwidth = 10;
        auto const IWzGr2D_Getheight = 11;
        auto const IWzGr2D_SetScreenResolution = 12;// guessed, inlined in CWvsApp::CallUpdate
        auto const IWzGr2D_Getfps100 = 15;
        auto const IWzGr2D_GetcurrentTime = 16;
        auto const IWzGr2D_GetfullScreen = 17;
        auto const IWzGr2D_SetFullScreen = 18;// guessed
        auto const IWzGr2D_SetSomeColor = 20; // guessed, inlined in CWvsApp::InitializeGr2D
        auto const IWzGr2D_GetredTone = 21;
        auto const IWzGr2D_GetgreenBlueTone = 22;
        auto const IWzGr2D_Getcenter = 23;
        auto const IWzGr2D_CreateLayer = 25;
        auto const IWzGr2D_PlayVideo = 30;
        auto const IWzGr2D_GetvideoStatus = 33;

        //
        // IWzGr2DLayer
        //
        auto const IWzGr2DLayer_Getalpha = 57;
        auto const IWzGr2DLayer_Getz = 44;

        //
        // IWzProperty: 986515d9_0a0b_4929_8b4f_718682177b92
        //
        auto const IWzProperty_Getitem = 5;
        auto const IWzProperty_Putitem = 6;

        //
        // IWzResMan: 57dfe40b_3e20_4dbc_97e8_805a50f381bf
        //

        // IWzCanvas: 7600dc6c_9328_4bff_9624_5b0f5c01179e

        // IWzVector2D: f28bd1ed_3deb_4f92_9eec_10ef5a1c3fb4

        // IWzFont: 2bef046d_ccd6_445a_88c4_929fc35d30ac

        // IWzSound: 1c923939_1338_4f8b_92cf_38935cee1fef

        // IWzNamespace: 2aeeeb36_a4e1_4e2b_8f6f_2e7bdec5c53d

        // IWzFileSystem: 352d8655_51e4_4668_8ce4_0866e2b6a5b5

        // IWzPackage: 352d8655_51e4_4668_8ce4_0866e2b6a5b5
    }// namespace vfunc
}// namespace address

#endif//MAPLE_2842A2F3CC18409CAE9C402C69931325_HPP
