#include "packets.hpp"

#include "../maple/packets.hpp"
#include "../maple/string.hpp"
#include "../util/memory/patch.hpp"

#include <fmt/core.h>
#include <string>
#include <unordered_map>
#include <vector>

auto const log_calls = true;

auto const CInPacket_CInPacket = 0x4E98AB;
auto const CInPacket_Decode1 = 0x00416004;
auto const CInPacket_Decode2 = 0x0041603A;
auto const CInPacket_Decode4 = 0x00416073;
auto const CInPacket_DecodeBuffer = 0x004160AB;
auto const CInPacket_DecodeStr = 0x0042DB40;

auto const CInPacket_staging = 0x66f8f8;

auto const COutPacket_COutPacket = 0x4E9CAF;
auto const COutPacket_Encode1 = 0x429F0C;
auto const COutPacket_Encode2 = 0x42DDA6;
auto const COutPacket_Encode4 = 0x429F69;
auto const COutPacket_EncodeStr = 0x42B5DA;
auto const COutPacket_EncodeBuffer = 0x42A193;

auto const CClientSocket_SendPacket = 0x43BF7F;
auto const CClientSocket_ProcessPacket_end = 0x0043C56F;

void install_packet_breakpoints() {
    for (auto addr: {COutPacket_COutPacket, CClientSocket_SendPacket, COutPacket_Encode1, COutPacket_Encode2,
                     COutPacket_Encode4, COutPacket_EncodeStr, COutPacket_EncodeBuffer, CInPacket_CInPacket,
                     CInPacket_Decode1, CInPacket_Decode2, CInPacket_Decode4, CInPacket_DecodeBuffer,
                     CInPacket_DecodeStr, CClientSocket_ProcessPacket_end}) {
        Patch(addr).int3();
    }
}

struct PacketSegment {
    virtual void print(fmt::memory_buffer &buffer) const = 0;

    [[nodiscard]] virtual uintptr_t length() const = 0;
};

template<typename T>
struct FixedPacketSegment : PacketSegment {
    T value;

    explicit FixedPacketSegment(T value) : value(value) {}

    void print(fmt::memory_buffer &buffer) const override {
        fmt::format_to(std::back_inserter(buffer),
                       "{} ({})", value, length());
    }

    [[nodiscard]] uintptr_t length() const override {
        return sizeof(T);
    }
};

struct StringPacketSegment : PacketSegment {
    std::string value;

    explicit StringPacketSegment(ZXString<char> const &str) : value(str.str) {}

    explicit StringPacketSegment(std::string_view str) : value(str) {}

    void print(fmt::memory_buffer &buffer) const override {
        fmt::format_to(std::back_inserter(buffer),
                       "\"{}\" ({})", value, value.length());
    }

    [[nodiscard]] uintptr_t length() const override {
        return 2 + value.length();
    }
};

struct BufferPacketSegment : PacketSegment {
    std::vector<uint8_t> value;

    BufferPacketSegment(uint8_t *buffer, size_t length) : value(buffer, buffer + length) {}

    void print(fmt::memory_buffer &buffer) const override {
        for (unsigned char i: value) {
            fmt::format_to(std::back_inserter(buffer),
                           "{:02x} ", i);
        }

        fmt::format_to(std::back_inserter(buffer),
                       "(buffer {})", value.size());
    }

    [[nodiscard]] uintptr_t length() const override {
        return value.size();
    }
};

std::unordered_map<uintptr_t, std::vector<std::unique_ptr<PacketSegment>>> outbound_packets{};

bool handle_out_packet_hooks(uintptr_t addr, PCONTEXT ctx) {
    if (addr == COutPacket_COutPacket) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const opcode = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);
        auto const a3 = *reinterpret_cast<uint32_t *>(ctx->Esp + 8);

        if (log_calls) {
            logger->trace("COutPacket::COutPacket(0x{:x}, 0x{:02x}, {}) from 0x{:x}",
                          packet, opcode, a3, ret_addr);
        }

        // Overwrite the segments if the object has already been initialized,
        // e.g. for movement packets that are updated every 500ms but only
        // sent when there's a change
        outbound_packets[packet] = std::vector<std::unique_ptr<PacketSegment>>{};

        // seg000:004E9CAF                 mov     eax, offset loc_610CCB
        // seg000:004E9CB4                 call    __EH_prolog
        ctx->Eax = 0x610CCB;
        ctx->Eip = 0x4E9CB4;

        return true;
    } else if (addr == CClientSocket_SendPacket) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const client_socket = ctx->Ecx;
        auto const packet = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);

        if (log_calls) {
            logger->trace("COutPacket::SendPacket(0x{:x}, 0x{:x}) from 0x{:x}",
                          client_socket, packet, ret_addr);
        }

        auto const out_packet = reinterpret_cast<COutPacket *>(packet);

        auto const &segments = outbound_packets.at(packet);

        fmt::memory_buffer buffer;
        size_t expected_length = 0;
        for (auto const &segment: segments) {
            fmt::format_to(std::back_inserter(buffer), "[");
            segment->print(buffer);
            fmt::format_to(std::back_inserter(buffer), "] ");
            expected_length += segment->length();
        }
        if (expected_length != out_packet->length) {
            logger->warning("Packet was sent with unknown segments (actual length: {}, expected length: {})",
                            out_packet->length, expected_length);
        }

        logger->trace("SEND {}", buffer.data());

        outbound_packets.erase(packet);

        // seg000:0043BF7F                 mov     eax, offset loc_6016A0
        // seg000:0043BF84                 call    __EH_prolog
        ctx->Eax = 0x6016A0;
        ctx->Eip = 0x43BF84;

        return true;
    } else if (addr == COutPacket_Encode1 || addr == COutPacket_Encode2 || addr == COutPacket_Encode4) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto value = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);

        std::unique_ptr<PacketSegment> segment;
        size_t length;
        if (addr == COutPacket_Encode1) {
            value &= 0xff;
            length = 1;
            segment = std::make_unique<FixedPacketSegment<uint8_t>>(value);
        } else if (addr == COutPacket_Encode2) {
            value &= 0xffff;
            length = 2;
            segment = std::make_unique<FixedPacketSegment<uint16_t>>(value);
        } else /*if (addr == COutPacket_Encode4)*/ {
            length = 4;
            segment = std::make_unique<FixedPacketSegment<uint32_t>>(value);
        }

        outbound_packets.at(packet).emplace_back(std::move(segment));

        if (log_calls) {
            logger->trace("COutPacket::Encode{}(0x{:x}, {}) from 0x{:x}",
                          length, packet, value, ret_addr);
        }

        // seg000:00429F0C                 push    esi
        // seg000:00429F0D                 mov     esi, ecx
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Esi;
        ctx->Eip = addr + 1;

        return true;
    } else if (addr == COutPacket_EncodeStr) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const value = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);
        auto const str = ZXString<char>::from_str(reinterpret_cast<char *>(value));

        if (log_calls) {
            logger->trace("COutPacket::EncodeStr(0x{:x}, \"{}\") from 0x{:x}", packet, str.str, ret_addr);
        }

        outbound_packets.at(packet).emplace_back(std::make_unique<StringPacketSegment>(str));

        // seg000:0042B5DA                 mov     eax, offset loc_5FFE54
        // seg000:0042B5DF                 call    __EH_prolog
        ctx->Eax = 0x5FFE54;
        ctx->Eip = 0x42B5DF;

        return true;
    } else if (addr == COutPacket_EncodeBuffer) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const buffer = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);
        auto const length = *reinterpret_cast<uint32_t *>(ctx->Esp + 8);

        if (log_calls) {
            logger->trace("COutPacket::EncodeBuffer(0x{:x}, 0x{:x}, {}) from 0x{:x}",
                          packet, buffer, length, ret_addr);
        }

        outbound_packets.at(packet)
                .emplace_back(std::make_unique<BufferPacketSegment>(reinterpret_cast<uint8_t *>(buffer), length));

        // seg000:0042A193                 push    esi
        // seg000:0042A194                 push    edi
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Esi;
        ctx->Eip = 0x42A194;

        return true;
    }

    return false;
}

std::vector<std::unique_ptr<PacketSegment>> inbound_packet{};

bool handle_in_packet_hooks(uintptr_t addr, PCONTEXT ctx) {
    if (addr == CInPacket_CInPacket) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const data = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);

        if (inbound_packet.size() > 0) {
            logger->error("Inbound packet wasn't cleared!");
            inbound_packet.clear();
        }

        if (log_calls) {
            logger->trace("CInPacket::CInPacket(0x{:x}, 0x{:x}) from 0x{:x} in {}",
                          packet, data, ret_addr, GetCurrentThreadId());
        }

        auto const bytes = *reinterpret_cast<uint8_t **>(data + 8);
        auto const length = *reinterpret_cast<uint16_t *>(data + 12);

        fmt::memory_buffer buffer;
        for (auto i = 0; i < length; i++) {
            fmt::format_to(std::back_inserter(buffer), "{:02x} ", bytes[i]);
        }

        logger->trace("CInPacket: {}", buffer.data());

        // seg000:004E98AB                 mov     eax, offset loc_610CB3
        // seg000:004E98B0                 call    __EH_prolog
        ctx->Eax = 0x610CB3;
        ctx->Eip = 0x4E98B0;

        return true;
    } else if (addr == CInPacket_Decode1) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;

        auto const data = *reinterpret_cast<uint8_t *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20));

        if (packet != CInPacket_staging) {
            inbound_packet.emplace_back(std::make_unique<FixedPacketSegment<uint8_t>>(data));
        }

        if (log_calls) {
            logger->trace("CInPacket::Decode1(0x{:x}) -> {} from 0x{:x}",
                          packet, data, ret_addr);
        }

        // seg000:00416004                 push    ebp
        // seg000:00416005                 mov     ebp, esp
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Ebp;
        ctx->Eip = addr + 1;

        return true;
    } else if (addr == CInPacket_Decode2) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;

        auto const data = *reinterpret_cast<uint16_t *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20));

        if (packet != CInPacket_staging) {
            inbound_packet.emplace_back(std::make_unique<FixedPacketSegment<uint16_t>>(data));
        }

        if (log_calls) {
            logger->trace("CInPacket::Decode2(0x{:x}) -> {} from 0x{:x}",
                          packet, data, ret_addr);
        }

        // seg000:0041603A                 push    ebp
        // seg000:0041603B                 mov     ebp, esp
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Ebp;
        ctx->Eip = addr + 1;

        return true;
    } else if (addr == CInPacket_Decode4) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;

        auto const data = *reinterpret_cast<uint32_t *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20));

        if (packet != CInPacket_staging) {
            inbound_packet.emplace_back(std::make_unique<FixedPacketSegment<uint32_t>>(data));
        }

        if (log_calls) {
            logger->trace("CInPacket::Decode4(0x{:x}) -> {} from 0x{:x}",
                          packet, data, ret_addr);
        }

        // seg000:00416004                 push    ebp
        // seg000:00416005                 mov     ebp, esp
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Ebp;
        ctx->Eip = addr + 1;

        return true;
    } else if (addr == CInPacket_DecodeStr) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const output = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);

        auto const length = *reinterpret_cast<uint16_t *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20));
        auto const string = reinterpret_cast<char *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20) +
                sizeof(uint16_t));

        if (packet != CInPacket_staging) {
            inbound_packet.emplace_back(std::make_unique<StringPacketSegment>(string));
        }

        if (log_calls) {
            logger->trace("CInPacket::DecodeStr(0x{:x}, 0x{:x}) -> \"{}\" ({}) from 0x{:x}",
                          packet, output, string, length, ret_addr);
        }

        // seg000:0042DB40                 mov     eax, offset loc_6002EF
        // seg000:0042DB45                 call    __EH_prolog
        ctx->Eax = 0x6002EF;
        ctx->Eip = 0x42DB45;

        return true;
    } else if (addr == CInPacket_DecodeBuffer) {
        auto const ret_addr = *reinterpret_cast<uint32_t *>(ctx->Esp);

        auto const packet = ctx->Ecx;
        auto const output = *reinterpret_cast<uint32_t *>(ctx->Esp + 4);
        auto const length = *reinterpret_cast<uint32_t *>(ctx->Esp + 8);

        auto const data = reinterpret_cast<uint8_t *>(
                *reinterpret_cast<uintptr_t *>(packet + 8) + *reinterpret_cast<uintptr_t *>(packet + 20));

        if (packet != CInPacket_staging) {
            inbound_packet.emplace_back(std::make_unique<BufferPacketSegment>(data, length));
        }

        if (log_calls) {
            logger->trace("CInPacket::DecodeBuffer(0x{:x}, 0x{:x}, {}) from 0x{:x}",
                          packet, output, length, ret_addr);
        }

        // seg000:004160AB                 push    ebp
        // seg000:004160AC                 mov     ebp, esp
        ctx->Esp -= sizeof(uint32_t);
        *reinterpret_cast<uint32_t *>(ctx->Esp) = ctx->Ebp;
        ctx->Eip = addr + 1;

        return true;
    } else if (addr == CClientSocket_ProcessPacket_end) {
        fmt::memory_buffer buffer;
        size_t expected_length = 0;
        for (auto const &segment: inbound_packet) {
            fmt::format_to(std::back_inserter(buffer), "[");
            segment->print(buffer);
            fmt::format_to(std::back_inserter(buffer), "] ");
            expected_length += segment->length();
        }

        logger->trace("RECV {}", buffer.data());

        inbound_packet.clear();

        // seg000:0043C56F                 pop     edi
        //seg000:0043C570                 pop     esi
        ctx->Edi = *reinterpret_cast<uint32_t *>(ctx->Esp);
        ctx->Esp += sizeof(uint32_t);
        ctx->Eip = addr + 1;

        return true;
    }


    return false;
}