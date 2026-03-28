// CpherTunnel - AnimatePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AnimatePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AnimatePacket, Animate)

    int32_t action = 0;
    ActorRuntimeID runtime_id;
    float row_boat = 0.0f; // only for RowRight/RowLeft

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action);
        stream.writeActorRuntimeID(runtime_id);
        if (action == 128 || action == 129) { // RowRight, RowLeft
            stream.writeFloat(row_boat);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readVarInt();
        runtime_id = stream.readActorRuntimeID();
        if (action == 128 || action == 129) {
            row_boat = stream.readFloat();
        }
    }
};

} // namespace cphertunnel
