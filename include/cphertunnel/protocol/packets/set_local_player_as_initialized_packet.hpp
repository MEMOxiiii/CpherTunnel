// CpherTunnel - SetLocalPlayerAsInitializedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetLocalPlayerAsInitializedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetLocalPlayerAsInitializedPacket, SetLocalPlayerAsInit)

    ActorRuntimeID runtime_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(runtime_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        runtime_entity_id = stream.readActorRuntimeID();
    }
};

} // namespace cphertunnel
