// CpherTunnel - ShowStoreOfferPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ShowStoreOfferPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ShowStoreOfferPacket, ShowStoreOffer)

    std::string offer_id;
    uint8_t show_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(offer_id);
        stream.writeByte(show_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        offer_id = stream.readString();
        show_type = stream.readByte();
    }
};

} // namespace cphertunnel
