// CpherTunnel - ActorEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ActorEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ActorEventPacket, ActorEvent)

    ActorRuntimeID runtime_id;
    uint8_t event_id = 0;
    int32_t data = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_id);
        stream.writeByte(event_id);
        stream.writeVarInt(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_id = stream.readActorRuntimeID();
        event_id = stream.readByte();
        data = stream.readVarInt();
    }
};

} // namespace cphertunnel
