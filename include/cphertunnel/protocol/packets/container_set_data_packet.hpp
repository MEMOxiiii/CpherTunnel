// CpherTunnel - ContainerSetDataPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ContainerSetDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ContainerSetDataPacket, ContainerSetData)

    uint8_t window_id = 0;
    int32_t property = 0;
    int32_t value = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(window_id);
        stream.writeVarInt(property);
        stream.writeVarInt(value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        window_id = stream.readByte();
        property = stream.readVarInt();
        value = stream.readVarInt();
    }
};

} // namespace cphertunnel
