// CpherTunnel - RemoveVolumeEntityPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RemoveVolumeEntityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RemoveVolumeEntityPacket, RemoveVolumeEntity)

    uint32_t entity_net_id = 0;
    int32_t dimension_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(entity_net_id);
        stream.writeVarInt(dimension_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_net_id = stream.readUnsignedVarInt();
        dimension_type = stream.readVarInt();
    }
};

} // namespace cphertunnel
