// CpherTunnel - UpdateSoftEnumPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


enum class SoftEnumUpdateType : uint8_t {
    Add     = 0,
    Remove  = 1,
    Replace = 2
};

class UpdateSoftEnumPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(UpdateSoftEnumPacket, UpdateSoftEnum)

    std::string enum_name;
    std::vector<std::string> values;
    SoftEnumUpdateType update_type = SoftEnumUpdateType::Replace;

    void write(BinaryStream& stream) const override {
        stream.writeString(enum_name);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(values.size()));
        for (auto& v : values) stream.writeString(v);
        stream.writeByte(static_cast<uint8_t>(update_type));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        enum_name = stream.readString();
        uint32_t count = stream.readUnsignedVarInt();
        values.resize(count);
        for (auto& v : values) v = stream.readString();
        update_type = static_cast<SoftEnumUpdateType>(stream.readByte());
    }
};

} // namespace cphertunnel
