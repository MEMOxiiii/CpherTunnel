// CpherTunnel - SetLastHurtByPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetLastHurtByPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetLastHurtByPacket, SetLastHurtBy)

    int32_t entity_type_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(entity_type_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_type_id = stream.readVarInt();
    }
};

} // namespace cphertunnel
