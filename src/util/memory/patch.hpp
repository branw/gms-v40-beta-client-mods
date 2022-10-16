#ifndef MAPLE_6AD7A1E8A13443F7BF77BA9EC80FCD77_HPP
#define MAPLE_6AD7A1E8A13443F7BF77BA9EC80FCD77_HPP

#include <Windows.h>
#include <cstddef>
#include <cstdint>
#include <string_view>

inline uint8_t operator""_u8(unsigned long long value) {
    return static_cast<uint8_t>(value);
}

enum class Reg {
    eax,
    ebx,
    ecx,
    edx,
    esp,
    ebp,
    esi,
    edi,
};

inline uint8_t reg_to_bits(Reg reg) {
    switch (reg) {
    default:
    case Reg::eax:
        return 0b000;
    case Reg::ecx:
        return 0b001;
    case Reg::edx:
        return 0b010;
    case Reg::ebx:
        return 0b011;
    case Reg::esp:
        return 0b100;
    case Reg::ebp:
        return 0b101;
    case Reg::esi:
        return 0b110;
    case Reg::edi:
        return 0b111;
    }
}

class Patch {
    std::size_t const MAX_PATCH_SIZE = 0x1000;

    uintptr_t start_address;
    uintptr_t address;

    unsigned long original_protection{};

public:
    explicit Patch(uintptr_t address) : start_address(address), address(address) {
        VirtualProtect(reinterpret_cast<void *>(start_address), MAX_PATCH_SIZE, PAGE_READWRITE, &original_protection);
    }

    ~Patch() {
        VirtualProtect(reinterpret_cast<void *>(start_address), MAX_PATCH_SIZE, original_protection,
                       &original_protection);

        if (start_address + MAX_PATCH_SIZE < address) {
            abort();
        }
    }

    template<typename T>
    Patch &write(T value) {
        *reinterpret_cast<T *>(address) = value;
        address += sizeof(T);
        return *this;
    }

    inline Patch &write(std::string_view str) {
        memcpy(reinterpret_cast<void *>(address), str.data(), str.length());
        address += str.length();
        return *this;
    }

    inline Patch &skip(std::size_t length) {
        address += length;
        return *this;
    }

    // jmp 0xdeadbeef -> jmp_rel(0xdeadbeef)
    inline Patch &jmp_rel(uintptr_t destination) {
        auto const current_address = address;
        write(0xe9_u8);
        write(destination - (current_address + 5));
        return *this;
    }

    inline Patch &jmp_rel_short(uintptr_t destination) {
        auto const current_address = address;
        write(0xeb_u8);
        write(destination - (current_address + 2));
        return *this;
    }

    inline Patch &nop(std::size_t length = 1) {
        for (auto i = 0; i < length; i++) {
            write(0x90_u8);
        }
        return *this;
    }

    inline Patch &int3() {
        write(0xcc_u8);
        return *this;
    }

    inline Patch &ret(uint16_t bytes_to_pop) {
        write(0xc2_u8);
        write(bytes_to_pop);
        return *this;
    }

    // push 0x10 -> push_imm(0x10)
    template<typename T>
    inline Patch &push_imm(T imm) {
        if constexpr (sizeof(T) == 1) {
            write(0x6a_u8);
        } else if constexpr (sizeof(T) == 2 || sizeof(T) == 4) {
            write(0x68_u8);
        }
        write(imm);
        return *this;
    }

    inline Patch &mov_reg_imm(Reg destination, uint32_t imm) {
        write<uint8_t>(0b10111000 | reg_to_bits(destination));
        write(imm);
        return *this;
    }

    // mov eax, [ebx+0x10] -> mov_reg_ireg_disp(Reg::eax, Reg::ebx, 0x10)
    inline Patch &mov_reg_ireg_disp(Reg destination, Reg source, uint8_t displacement) {
        write(0x8b_u8);
        write<uint8_t>(0b0100'0000 | (reg_to_bits(destination) << 3) | reg_to_bits(source));
        write(displacement);
        return *this;
    }

    // lea ebx, [ecx+0x12345678] -> lea_reg_ireg_disp(Reg::ebx, Reg::ecx, 0x12345678)
    inline Patch &lea_reg_ireg_disp(Reg destination, Reg source, uint32_t displacement) {
        write(0x8d_u8);
        write<uint8_t>(0b1000'0000 | (reg_to_bits(destination) << 3) | reg_to_bits(source));
        write(displacement);
        return *this;
    }
};

#endif//MAPLE_6AD7A1E8A13443F7BF77BA9EC80FCD77_HPP
