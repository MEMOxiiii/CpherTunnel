// CpherTunnel - ChangeMobPropertyPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ChangeMobPropertyPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ChangeMobPropertyPacket, ChangeMobProperty)

    ActorUniqueID entity_unique_id;
    std::string property;
    bool bool_value = false;
    std::string string_value;
    int32_t int_value = 0;
    float float_value = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeString(property);
        stream.writeBool(bool_value);
        stream.writeString(string_value);
        stream.writeVarInt(int_value);
        stream.writeFloat(float_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readActorUniqueID();
        property = stream.readString();
        bool_value = stream.readBool();
        string_value = stream.readString();
        int_value = stream.readVarInt();
        float_value = stream.readFloat();
    }
};

} // namespace cphertunnel
