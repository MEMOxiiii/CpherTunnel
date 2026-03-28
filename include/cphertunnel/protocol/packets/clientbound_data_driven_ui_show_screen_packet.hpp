// CpherTunnel - ClientboundDataDrivenUIShowScreenPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundDataDrivenUIShowScreenPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundDataDrivenUIShowScreenPacket, ClientboundDataDrivenUIShowScreen)

    std::string screen_name;
    std::string screen_data;

    void write(BinaryStream& stream) const override {
        stream.writeString(screen_name);
        stream.writeString(screen_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        screen_name = stream.readString();
        screen_data = stream.readString();
    }
};

} // namespace cphertunnel
