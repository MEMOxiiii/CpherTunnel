// CpherTunnel - DebugInfoPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class DebugInfoPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(DebugInfoPacket, DebugInfo)

    ActorUniqueID entity_id;
    std::string data;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
        stream.writeString(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
        data = stream.readString();
    }
};

} // namespace cphertunnel
