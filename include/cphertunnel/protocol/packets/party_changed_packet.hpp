// CpherTunnel - PartyChangedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PartyChangedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PartyChangedPacket, PartyChanged)

    std::string party_id;

    void write(BinaryStream& stream) const override {
        stream.writeString(party_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        party_id = stream.readString();
    }
};

} // namespace cphertunnel
