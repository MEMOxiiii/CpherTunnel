// CpherTunnel - ShowProfilePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ShowProfilePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ShowProfilePacket, ShowProfile)

    std::string xuid;

    void write(BinaryStream& stream) const override {
        stream.writeString(xuid);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        xuid = stream.readString();
    }
};

} // namespace cphertunnel
