// CpherTunnel - ClientMovementPredictionSyncPacket (ID: 322)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
#include <bitset>
namespace cphertunnel {
class ClientMovementPredictionSyncPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientMovementPredictionSyncPacket, ClientMovementPredictionSync)

    // ActorDataFlagComponent - 127-bit bitset serialized as 2 uint64
    uint64_t flags_low = 0;
    uint64_t flags_high = 0;
    // Bounding box
    float bb_scale = 1.0f, bb_width = 0.6f, bb_height = 1.8f;
    // Movement attributes
    MovementAttributesComponent movement;
    ActorUniqueID actor_unique_id;
    bool is_flying = false;

    void write(BinaryStream& s) const override {
        s.writeUnsignedInt64(flags_low);
        s.writeUnsignedInt64(flags_high);
        s.writeFloat(bb_scale);
        s.writeFloat(bb_width);
        s.writeFloat(bb_height);
        movement.write(s);
        s.writeActorUniqueID(actor_unique_id);
        s.writeBool(is_flying);
    }
    void read(ReadOnlyBinaryStream& s) override {
        flags_low = s.readUnsignedInt64();
        flags_high = s.readUnsignedInt64();
        bb_scale = s.readFloat();
        bb_width = s.readFloat();
        bb_height = s.readFloat();
        movement.read(s);
        actor_unique_id = s.readActorUniqueID();
        is_flying = s.readBool();
    }
};
} // namespace cphertunnel
