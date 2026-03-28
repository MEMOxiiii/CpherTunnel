// CpherTunnel - PlayStatusPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayStatusPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayStatusPacket, PlayStatus)

    PlayStatusType status = PlayStatusType::LoginSuccess;

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(static_cast<int32_t>(status));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        status = static_cast<PlayStatusType>(stream.readSignedBigEndianInt());
    }
};

} // namespace cphertunnel
