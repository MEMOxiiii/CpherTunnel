// CpherTunnel - ServerboundPackSettingChangePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerboundPackSettingChangePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerboundPackSettingChangePacket, ServerboundPackSettingChange)

    cphertunnel::UUID pack_id;
    std::string pack_setting_name;
    // value can be float, boolean, or string — stored as variant bytes
    ByteBuffer setting_value;

    void write(BinaryStream& stream) const override {
        stream.writeUUID(pack_id);
        stream.writeString(pack_setting_name);
        stream.writeRawBytes(setting_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        pack_id = stream.readUUID();
        pack_setting_name = stream.readString();
        setting_value = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
