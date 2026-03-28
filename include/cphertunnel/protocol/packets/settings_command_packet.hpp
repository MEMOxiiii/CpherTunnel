// CpherTunnel - SettingsCommandPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SettingsCommandPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SettingsCommandPacket, SettingsCommand)

    std::string command;
    bool suppress_output = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(command);
        stream.writeBool(suppress_output);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        command = stream.readString();
        suppress_output = stream.readBool();
    }
};

} // namespace cphertunnel
