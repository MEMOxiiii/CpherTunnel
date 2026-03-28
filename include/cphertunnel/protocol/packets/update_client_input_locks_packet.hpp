// CpherTunnel - UpdateClientInputLocksPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class UpdateClientInputLocksPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateClientInputLocksPacket, PlayerClientInputPermissions)

    uint32_t lock_component_data = 0;
    Vec3 server_position;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(lock_component_data);
        stream.writeVec3(server_position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        lock_component_data = stream.readUnsignedVarInt();
        server_position = stream.readVec3();
    }
};

} // namespace cphertunnel
