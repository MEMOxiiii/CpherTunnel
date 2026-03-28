// CpherTunnel - PlayerUpdateEntityOverridesPacket (ID: 325)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class PlayerUpdateEntityOverridesPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerUpdateEntityOverridesPacket, PlayerUpdateEntityOverrides)

    enum class UpdateType : uint8_t { ClearOverride = 0, SetIntOverride = 1, SetFloatOverride = 2 };

    ActorUniqueID target_id;
    uint32_t property_index = 0;
    UpdateType update_type = UpdateType::ClearOverride;
    int32_t int_value = 0;
    float float_value = 0.0f;

    void write(BinaryStream& s) const override {
        s.writeActorUniqueID(target_id);
        s.writeUnsignedVarInt(property_index);
        s.writeByte(static_cast<uint8_t>(update_type));
        if (update_type == UpdateType::SetIntOverride) s.writeSignedInt(int_value);
        else if (update_type == UpdateType::SetFloatOverride) s.writeFloat(float_value);
    }
    void read(ReadOnlyBinaryStream& s) override {
        target_id = s.readActorUniqueID();
        property_index = s.readUnsignedVarInt();
        update_type = static_cast<UpdateType>(s.readByte());
        if (update_type == UpdateType::SetIntOverride) int_value = s.readSignedInt();
        else if (update_type == UpdateType::SetFloatOverride) float_value = s.readFloat();
    }
};
} // namespace cphertunnel
