// CpherTunnel - PositionTrackingDBServerBroadcastPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PositionTrackingDBServerBroadcastPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PositionTrackingDBServerBroadcastPacket, PositionTrackingDBServerBroadcast)

    enum class Action : uint8_t {
        Update   = 0,
        Destroy  = 1,
        NotFound = 2,
    };

    Action action = Action::Update;
    int32_t tracking_id = 0;
    ByteBuffer position_data_nbt; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(action));
        stream.writeVarInt(tracking_id);
        stream.writeRawBytes(position_data_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = static_cast<Action>(stream.readByte());
        tracking_id = stream.readVarInt();
        position_data_nbt = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
