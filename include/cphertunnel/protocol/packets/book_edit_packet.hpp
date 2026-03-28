// CpherTunnel - BookEditPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class BookEditPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(BookEditPacket, BookEdit)

    uint8_t action = 0;
    uint8_t inventory_slot = 0;
    // Fields depend on action type
    uint8_t page_number = 0;
    uint8_t secondary_page_number = 0;
    std::string text;
    std::string photo;
    std::string title;
    std::string author;
    std::string xuid;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeByte(inventory_slot);
        switch (action) {
            case 0: // ReplacePage
            case 1: // AddPage
                stream.writeByte(page_number);
                stream.writeString(text);
                stream.writeString(photo);
                break;
            case 2: // DeletePage
                stream.writeByte(page_number);
                break;
            case 3: // SwapPages
                stream.writeByte(page_number);
                stream.writeByte(secondary_page_number);
                break;
            case 4: // SignBook
                stream.writeString(title);
                stream.writeString(author);
                stream.writeString(xuid);
                break;
            default: break;
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        inventory_slot = stream.readByte();
        switch (action) {
            case 0:
            case 1:
                page_number = stream.readByte();
                text = stream.readString();
                photo = stream.readString();
                break;
            case 2:
                page_number = stream.readByte();
                break;
            case 3:
                page_number = stream.readByte();
                secondary_page_number = stream.readByte();
                break;
            case 4:
                title = stream.readString();
                author = stream.readString();
                xuid = stream.readString();
                break;
            default: break;
        }
    }
};

} // namespace cphertunnel
