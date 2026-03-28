// CpherTunnel - CompletedUsingItemPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CompletedUsingItemPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CompletedUsingItemPacket, CompletedUsingItem)

    int16_t item_id = 0;
    int32_t use_method = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedShort(item_id);
        stream.writeSignedInt(use_method);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        item_id = stream.readSignedShort();
        use_method = stream.readSignedInt();
    }
};

} // namespace cphertunnel
