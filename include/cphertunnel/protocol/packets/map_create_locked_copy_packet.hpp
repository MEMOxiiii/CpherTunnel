// CpherTunnel - MapCreateLockedCopyPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class MapCreateLockedCopyPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(MapCreateLockedCopyPacket, MapCreateLockedCopy)

    ActorUniqueID original_map_id;
    ActorUniqueID new_map_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(original_map_id);
        stream.writeActorUniqueID(new_map_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        original_map_id = stream.readActorUniqueID();
        new_map_id = stream.readActorUniqueID();
    }
};

} // namespace cphertunnel
