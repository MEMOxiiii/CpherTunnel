// CpherTunnel - ClientboundDataDrivenUICloseScreenPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundDataDrivenUICloseScreenPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundDataDrivenUICloseScreenPacket, ClientboundDataDrivenUICloseScreen)

    std::optional<uint32_t> form_id;

    void write(BinaryStream& stream) const override {
        stream.writeBool(form_id.has_value());
        if (form_id.has_value()) {
            stream.writeUnsignedInt(form_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        if (stream.readBool()) {
            form_id = stream.readUnsignedInt();
        }
    }
};

} // namespace cphertunnel
