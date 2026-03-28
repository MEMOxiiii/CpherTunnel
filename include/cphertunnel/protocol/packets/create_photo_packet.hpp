// CpherTunnel - CreatePhotoPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CreatePhotoPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CreatePhotoPacket, CreatePhoto)

    int64_t entity_unique_id = 0;
    std::string photo_name;
    std::string item_name;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(entity_unique_id);
        stream.writeString(photo_name);
        stream.writeString(item_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readSignedInt64();
        photo_name = stream.readString();
        item_name = stream.readString();
    }
};

} // namespace cphertunnel
