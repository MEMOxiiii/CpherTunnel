// CpherTunnel - LegacyTelemetryEventPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class LegacyTelemetryEventPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(LegacyTelemetryEventPacket, LegacyTelemetryEvent)

    ActorUniqueID target_actor_id;
    int32_t event_type = 0;
    bool use_player_id = false;
    // Event data varies by event_type; stored as raw bytes for the variant payload
    ByteBuffer event_data;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(target_actor_id);
        stream.writeVarInt(event_type);
        stream.writeBool(use_player_id);
        stream.writeRawBytes(event_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        target_actor_id = stream.readActorUniqueID();
        event_type = stream.readVarInt();
        use_player_id = stream.readBool();
        event_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
