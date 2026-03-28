// CpherTunnel - ScriptMessagePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ScriptMessagePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ScriptMessagePacket, ScriptMessage)

    std::string message_id;
    std::string message_value;

    void write(BinaryStream& stream) const override {
        stream.writeString(message_id);
        stream.writeString(message_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        message_id = stream.readString();
        message_value = stream.readString();
    }
};

} // namespace cphertunnel
