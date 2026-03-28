// CpherTunnel - UpdateAbilitiesPacket (ID: 187)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class UpdateAbilitiesPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateAbilitiesPacket, UpdateAbilities)
    SerializedAbilitiesData data;

    void write(BinaryStream& s) const override { data.write(s); }
    void read(ReadOnlyBinaryStream& s) override { data.read(s); }
};
} // namespace cphertunnel
