// CpherTunnel - ModalFormResponsePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ModalFormResponsePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ModalFormResponsePacket, ModalFormResponse)

    uint32_t form_id = 0;
    bool has_response_data = false;
    std::string response_json;
    bool has_cancel_reason = false;
    uint8_t cancel_reason = 0; // 0=user_closed, 1=user_busy

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(form_id);
        stream.writeBool(has_response_data);
        if (has_response_data) {
            stream.writeString(response_json);
        }
        stream.writeBool(has_cancel_reason);
        if (has_cancel_reason) {
            stream.writeByte(cancel_reason);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedVarInt();
        has_response_data = stream.readBool();
        if (has_response_data) {
            response_json = stream.readString();
        }
        has_cancel_reason = stream.readBool();
        if (has_cancel_reason) {
            cancel_reason = stream.readByte();
        }
    }
};

} // namespace cphertunnel
