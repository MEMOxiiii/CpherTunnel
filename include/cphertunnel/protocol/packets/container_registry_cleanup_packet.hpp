// CpherTunnel - ContainerRegistryCleanupPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ContainerRegistryCleanupPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ContainerRegistryCleanupPacket, ContainerRegistryCleanup)

    std::vector<uint8_t> removed_containers;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(removed_containers.size()));
        for (auto c : removed_containers) {
            stream.writeByte(c);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        removed_containers.resize(count);
        for (auto& c : removed_containers) {
            c = stream.readByte();
        }
    }
};

} // namespace cphertunnel
