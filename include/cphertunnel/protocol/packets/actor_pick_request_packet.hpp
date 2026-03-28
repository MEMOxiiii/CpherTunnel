// CpherTunnel - ActorPickRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ActorPickRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ActorPickRequestPacket, ActorPickRequest)

    int64_t entity_id = 0;
    uint8_t hotbar_slot = 0;
    bool with_data = false;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(entity_id);
        stream.writeByte(hotbar_slot);
        stream.writeBool(with_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readSignedInt64();
        hotbar_slot = stream.readByte();
        with_data = stream.readBool();
    }
};

} // namespace cphertunnel
