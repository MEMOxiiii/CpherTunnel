// CpherTunnel - SetCommandsEnabledPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetCommandsEnabledPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetCommandsEnabledPacket, SetCommandsEnabled)

    bool commands_enabled = true;

    void write(BinaryStream& stream) const override {
        stream.writeBool(commands_enabled);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        commands_enabled = stream.readBool();
    }
};

} // namespace cphertunnel
