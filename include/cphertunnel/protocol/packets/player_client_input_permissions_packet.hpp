// CpherTunnel - PlayerClientInputPermissionsPacket (ID: 196)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PlayerClientInputPermissionsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerClientInputPermissionsPacket, PlayerClientInputPermissions)
    // Permissions bitfield matching InputLockFlags
    uint32_t permission_flags = 0;

    void write(BinaryStream& s) const override { s.writeUnsignedVarInt(permission_flags); }
    void read(ReadOnlyBinaryStream& s) override { permission_flags = s.readUnsignedVarInt(); }
};
} // namespace cphertunnel
