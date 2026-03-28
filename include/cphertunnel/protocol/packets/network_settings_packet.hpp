// CpherTunnel - NetworkSettingsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class NetworkSettingsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(NetworkSettingsPacket, NetworkSettings)

    uint16_t compression_threshold = 1;
    CompressionAlgorithm compression_algorithm = CompressionAlgorithm::ZLib;
    bool client_throttle_enabled = false;
    uint8_t client_throttle_threshold = 0;
    float client_throttle_scalar = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedShort(compression_threshold);
        stream.writeUnsignedShort(static_cast<uint16_t>(compression_algorithm));
        stream.writeBool(client_throttle_enabled);
        stream.writeByte(client_throttle_threshold);
        stream.writeFloat(client_throttle_scalar);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        compression_threshold = stream.readUnsignedShort();
        compression_algorithm = static_cast<CompressionAlgorithm>(stream.readUnsignedShort());
        client_throttle_enabled = stream.readBool();
        client_throttle_threshold = stream.readByte();
        client_throttle_scalar = stream.readFloat();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }

    bool disallowBatching() const override { return true; }
};

} // namespace cphertunnel
