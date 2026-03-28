// CpherTunnel - AvailableCommandsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class AvailableCommandsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(AvailableCommandsPacket, AvailableCommands)

    // The AvailableCommands packet has one of the most complex serialization
    // formats in the protocol, with interlinked enum/command/postfix tables.
    // We store the raw payload for maximum forward compatibility.
    ByteBuffer commands_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(commands_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        commands_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
