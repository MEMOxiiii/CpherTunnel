// CpherTunnel - RequestAbilityPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class RequestAbilityPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RequestAbilityPacket, RequestAbility)

    enum class ValueType : uint8_t {
        Unset = 0,
        Bool  = 1,
        Float = 2,
    };

    int32_t ability = 0;
    ValueType value_type = ValueType::Unset;
    bool bool_value = false;
    float float_value = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(ability);
        stream.writeByte(static_cast<uint8_t>(value_type));
        stream.writeBool(bool_value);
        stream.writeFloat(float_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        ability = stream.readVarInt();
        value_type = static_cast<ValueType>(stream.readByte());
        bool_value = stream.readBool();
        float_value = stream.readFloat();
    }
};

} // namespace cphertunnel
