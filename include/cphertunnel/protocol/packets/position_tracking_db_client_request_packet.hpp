// CpherTunnel - PositionTrackingDBClientRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PositionTrackingDBClientRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PositionTrackingDBClientRequestPacket, PositionTrackingDBClientRequest)

    uint8_t action = 0; // 0=Query
    int32_t tracking_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeVarInt(tracking_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        tracking_id = stream.readVarInt();
    }
};

} // namespace cphertunnel
