// CpherTunnel - RequestPermissionsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RequestPermissionsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RequestPermissionsPacket, RequestPermissions)

    int64_t target_player_raw_id = 0;
    int32_t player_permission_level = 0;
    uint16_t custom_permission_flags = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(target_player_raw_id);
        stream.writeVarInt(player_permission_level);
        stream.writeUnsignedShort(custom_permission_flags);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        target_player_raw_id = stream.readSignedInt64();
        player_permission_level = stream.readVarInt();
        custom_permission_flags = stream.readUnsignedShort();
    }
};

} // namespace cphertunnel
