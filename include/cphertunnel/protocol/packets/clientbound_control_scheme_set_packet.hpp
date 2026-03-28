// CpherTunnel - ClientboundControlSchemeSetPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundControlSchemeSetPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundControlSchemeSetPacket, ClientboundControlSchemeSet)

    uint8_t control_scheme = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(control_scheme);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        control_scheme = stream.readByte();
    }
};

} // namespace cphertunnel
