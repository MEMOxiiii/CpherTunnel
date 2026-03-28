// CpherTunnel - PurchaseReceiptPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class PurchaseReceiptPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(PurchaseReceiptPacket, PurchaseReceipt)

    std::vector<std::string> receipt_ids;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(receipt_ids.size()));
        for (auto& r : receipt_ids) stream.writeString(r);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        receipt_ids.resize(count);
        for (auto& r : receipt_ids) r = stream.readString();
    }
};

} // namespace cphertunnel
