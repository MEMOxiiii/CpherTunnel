// CpherTunnel - PlayerSkinPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerSkinPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerSkinPacket, PlayerSkin)

    UUID player_uuid;
    // Skin data is complex - store as raw for forward compatibility
    ByteBuffer skin_data;

    void write(BinaryStream& stream) const override {
        stream.writeUUID(player_uuid);
        stream.writeRawBytes(skin_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        player_uuid = stream.readUUID();
        skin_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
