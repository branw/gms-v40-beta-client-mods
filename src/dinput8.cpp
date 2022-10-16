#include "dinput8.hpp"

namespace dinput8 {
    DirectInput8Create_t original;

    void init() {
        char library_path[MAX_PATH];
        GetSystemDirectoryA(library_path, MAX_PATH);
        strcat_s(library_path, "\\dinput8.dll");

        auto const module = LoadLibraryA(library_path);
        if (module) {
            original = reinterpret_cast<DirectInput8Create_t>(GetProcAddress(module, "DirectInput8Create"));
        }
    }

    extern "C" __declspec(dllexport)
            HRESULT DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter) {
        if (original) {
            return original(hinst, dwVersion, riidltf, ppvOut, punkOuter);
        }

        return S_FALSE;
    }
}// namespace dinput8