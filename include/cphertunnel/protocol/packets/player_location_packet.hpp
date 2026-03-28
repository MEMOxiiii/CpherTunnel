// CpherTunnel - PlayerLocationPacket (ID: 326)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PlayerLocationPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerLocationPacket, PlayerLocation)

    enum class Type : int32_t { Coordinates = 0, Hide = 1 };

    Type type = Type::Coordinates;
    ActorUniqueID target_actor_id;
    Vec3 position{0, 0, 0};  // only for Coordinates

    void write(BinaryStream& s) const override {
        s.writeSignedInt(static_cast<int32_t>(type));
        s.writeActorUniqueID(target_actor_id);
        if (type == Type::Coordinates) s.writeVec3(position);
    }
    void read(ReadOnlyBinaryStream& s) override {
        type = static_cast<Type>(s.readSignedInt());
        target_actor_id = s.readActorUniqueID();
        if (type == Type::Coordinates) position = s.readVec3();
    }
};
} // namespace cphertunnel
