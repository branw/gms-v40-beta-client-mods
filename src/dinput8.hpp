#ifndef MAPLE_2129D32FF43E477BBEDF5A6B79A5E3B1_HPP
#define MAPLE_2129D32FF43E477BBEDF5A6B79A5E3B1_HPP

#include <Windows.h>

namespace dinput8 {
    void init();

    typedef HRESULT (*DirectInput8Create_t)(
            HINSTANCE hinst,
            DWORD dwVersion,
            REFIID riidltf,
            LPVOID *ppvOut,
            LPUNKNOWN punkOuter);

    extern DirectInput8Create_t original;

    extern "C" __declspec(dllexport)
            HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
}// namespace dinput8

#endif//MAPLE_2129D32FF43E477BBEDF5A6B79A5E3B1_HPP
