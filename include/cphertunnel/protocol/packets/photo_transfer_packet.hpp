// CpherTunnel - PhotoTransferPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PhotoTransferPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PhotoTransferPacket, PhotoTransfer)

    std::string photo_name;
    std::string photo_data;
    std::string book_id;
    uint8_t photo_type = 0;
    uint8_t source_type = 0;
    int64_t owner_entity_unique_id = 0;
    std::string new_photo_name;

    void write(BinaryStream& stream) const override {
        stream.writeString(photo_name);
        stream.writeString(photo_data);
        stream.writeString(book_id);
        stream.writeByte(photo_type);
        stream.writeByte(source_type);
        stream.writeSignedInt64(owner_entity_unique_id);
        stream.writeString(new_photo_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        photo_name = stream.readString();
        photo_data = stream.readString();
        book_id = stream.readString();
        photo_type = stream.readByte();
        source_type = stream.readByte();
        owner_entity_unique_id = stream.readSignedInt64();
        new_photo_name = stream.readString();
    }
};

} // namespace cphertunnel
