// CpherTunnel - ServerboundLoadingScreenPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerboundLoadingScreenPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerboundLoadingScreenPacket, ServerboundLoadingScreen)

    int32_t type = 0;
    std::optional<uint32_t> loading_screen_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(type);
        stream.writeBool(loading_screen_id.has_value());
        if (loading_screen_id.has_value()) {
            stream.writeUnsignedInt(loading_screen_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        type = stream.readVarInt();
        if (stream.readBool()) {
            loading_screen_id = stream.readUnsignedInt();
        }
    }
};

} // namespace cphertunnel
