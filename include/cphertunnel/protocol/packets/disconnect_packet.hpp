// CpherTunnel - DisconnectPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class DisconnectPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(DisconnectPacket, Disconnect)

    DisconnectFailReason reason = DisconnectFailReason::Unknown;
    bool skip_message = false;
    std::string message;
    std::string filtered_message;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(static_cast<int32_t>(reason));
        stream.writeBool(skip_message);
        if (!skip_message) {
            stream.writeString(message);
            stream.writeString(filtered_message);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        reason = static_cast<DisconnectFailReason>(stream.readVarInt());
        skip_message = stream.readBool();
        if (!skip_message) {
            message = stream.readString();
            filtered_message = stream.readString();
        }
    }
};

} // namespace cphertunnel
