// CpherTunnel - ModalFormRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ModalFormRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ModalFormRequestPacket, ModalFormRequest)

    uint32_t form_id = 0;
    std::string form_json; // JSON string for the form

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(form_id);
        stream.writeString(form_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedVarInt();
        form_json = stream.readString();
    }
};

} // namespace cphertunnel
