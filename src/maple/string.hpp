#ifndef MAPLE_24C91C09EF134CDC82E3B245188FA0A8_HPP
#define MAPLE_24C91C09EF134CDC82E3B245188FA0A8_HPP

#include "../util/logging/logger.hpp"

#include <array>
#include <cstdint>

template<typename T>
struct ZXString {
    struct Metadata {
        uint32_t ref_count;
        uint32_t capacity;
        uint32_t length;

        Metadata(uint32_t ref_count, uint32_t capacity, uint32_t length) : ref_count(ref_count), capacity(capacity),
                                                                           length(length) {}
    };

    T *str;

    static ZXString<T> from_str(T *str) {
        return {str};
    }

    static ZXString<T> create(std::string_view input) {
        auto const size = sizeof(Metadata) + input.length() + 1;
        auto result = new uint8_t[size];
        auto metadata = reinterpret_cast<Metadata *>(result);
        auto str = reinterpret_cast<char *>(result + sizeof(Metadata));

        std::copy(input.begin(), input.end(), str);
        result[size - 1] = '\0';

        return {str};
    }

    Metadata *metadata() {
        return reinterpret_cast<Metadata *>(reinterpret_cast<uintptr_t>(this) - offsetof(ZXString<T>, str));
    }

    [[nodiscard]] uint32_t length() const {
        return metadata()->length;
    }
};

static_assert(sizeof(ZXString<char>) == sizeof(uintptr_t));
static_assert(sizeof(typename ZXString<char>::Metadata) == 12);

template<typename T, size_t Size>
struct StaticZXString {
    typename ZXString<T>::Metadata metadata;
    std::array<T, Size> data;

    explicit constexpr StaticZXString(T const (&str)[Size], uint32_t ref_count = 1) : data(std::to_array(str)), metadata(ref_count, Size - 1, Size - 1) {}

    [[nodiscard]] T *to_str() {
        return reinterpret_cast<T *>(data.data());
    }
};

static_assert(sizeof(StaticZXString<char, 4>) == 12 + 4);

#endif//MAPLE_24C91C09EF134CDC82E3B245188FA0A8_HPP
