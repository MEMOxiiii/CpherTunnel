// CpherTunnel - TakeItemActorPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class TakeItemActorPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(TakeItemActorPacket, TakeItemActor)

    ActorRuntimeID item_runtime_id;
    ActorRuntimeID player_runtime_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(item_runtime_id);
        stream.writeActorRuntimeID(player_runtime_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_runtime_id = stream.readActorRuntimeID();
        player_runtime_id = stream.readActorRuntimeID();
    }
};

} // namespace cphertunnel
