// CpherTunnel - UpdateAttributesPacket (ID: 29)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {
class UpdateAttributesPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateAttributesPacket, UpdateAttributes)
    ActorRuntimeID runtime_id;
    std::vector<AttributeData> attributes;
    PlayerInputTick tick;

    void write(BinaryStream& s) const override {
        s.writeActorRuntimeID(runtime_id);
        s.writeUnsignedVarInt(static_cast<uint32_t>(attributes.size()));
        for (auto& a : attributes) a.writeFull(s);
        tick.write(s);
    }
    void read(ReadOnlyBinaryStream& s) override {
        runtime_id = s.readActorRuntimeID();
        uint32_t count = s.readUnsignedVarInt();
        attributes.resize(count);
        for (auto& a : attributes) a.readFull(s);
        tick.read(s);
    }
};
} // namespace cphertunnel
