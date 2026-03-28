// CpherTunnel - ShowCreditsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ShowCreditsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ShowCreditsPacket, ShowCredits)

    ActorRuntimeID player_id;
    int32_t status = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorRuntimeID(player_id);
        stream.writeVarInt(status);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        player_id = stream.readActorRuntimeID();
        status = stream.readVarInt();
    }
};

} // namespace cphertunnel
