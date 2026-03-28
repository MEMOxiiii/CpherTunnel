// CpherTunnel - PlayerVideoCapturePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PlayerVideoCapturePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PlayerVideoCapturePacket, PlayerVideoCapture)

    uint8_t action = 0;
    ByteBuffer capture_data;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeRawBytes(capture_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        capture_data = stream.readRemainingBytes();
    }
};

} // namespace cphertunnel
