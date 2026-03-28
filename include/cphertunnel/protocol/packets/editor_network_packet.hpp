// CpherTunnel - EditorNetworkPacket
#pragma once
#include "../packet.hpp"
#include "../nbt.hpp"
namespace cphertunnel {

class EditorNetworkPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(EditorNetworkPacket, EditorNetwork)

    std::shared_ptr<NbtTag> editor_data;

    void write(BinaryStream& stream) const override {
        if (editor_data) {
            NbtTag::writeNetworkNbt(stream, *editor_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        editor_data = NbtTag::readNetworkNbt(stream);
    }
};

} // namespace cphertunnel
