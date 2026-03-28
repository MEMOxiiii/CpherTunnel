// CpherTunnel - ContainerClosePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ContainerClosePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ContainerClosePacket, ContainerClose)

    uint8_t window_id = 0;
    uint8_t container_type = 0;
    bool server_initiated = false;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeByte(container_type);
        stream.writeBool(server_initiated);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        container_type = stream.readByte();
        server_initiated = stream.readBool();
    }
};

} // namespace cphertunnel
