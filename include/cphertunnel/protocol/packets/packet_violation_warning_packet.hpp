// CpherTunnel - PacketViolationWarningPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PacketViolationWarningPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PacketViolationWarningPacket, PacketViolationWarning)

    int32_t violation_type = 0;
    int32_t severity = 0;
    int32_t packet_id_cause = 0;
    std::string context;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(violation_type);
        stream.writeVarInt(severity);
        stream.writeVarInt(packet_id_cause);
        stream.writeString(context);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        violation_type = stream.readVarInt();
        severity = stream.readVarInt();
        packet_id_cause = stream.readVarInt();
        context = stream.readString();
    }
};

} // namespace cphertunnel
