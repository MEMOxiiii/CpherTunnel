// CpherTunnel - ResourcePackClientResponsePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ResourcePackClientResponsePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ResourcePackClientResponsePacket, ResourcePackClientResponse)

    ResourcePackResponseType response = ResourcePackResponseType::None;
    std::vector<std::string> pack_ids;

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(response));
        stream.writeUnsignedShort(static_cast<uint16_t>(pack_ids.size()));
        for (auto& id : pack_ids) {
            stream.writeString(id);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        response = static_cast<ResourcePackResponseType>(stream.readByte());
        uint16_t count = stream.readUnsignedShort();
        pack_ids.resize(count);
        for (auto& id : pack_ids) {
            id = stream.readString();
        }
    }
};

} // namespace cphertunnel
