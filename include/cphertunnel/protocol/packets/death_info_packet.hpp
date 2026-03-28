// CpherTunnel - DeathInfoPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class DeathInfoPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(DeathInfoPacket, DeathInfo)

    std::string death_cause_message;
    std::vector<std::string> death_cause_params;

    void write(BinaryStream& stream) const override {
        stream.writeString(death_cause_message);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(death_cause_params.size()));
        for (auto& p : death_cause_params) stream.writeString(p);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        death_cause_message = stream.readString();
        uint32_t count = stream.readUnsignedVarInt();
        death_cause_params.resize(count);
        for (auto& p : death_cause_params) p = stream.readString();
    }
};

} // namespace cphertunnel
