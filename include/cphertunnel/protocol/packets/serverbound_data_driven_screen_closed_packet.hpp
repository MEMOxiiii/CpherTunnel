// CpherTunnel - ServerboundDataDrivenScreenClosedPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ServerboundDataDrivenScreenClosedPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ServerboundDataDrivenScreenClosedPacket, ServerboundDataDrivenScreenClosed)

    enum class CloseReason : uint32_t {
        ProgrammaticClose    = 0,
        ProgrammaticCloseAll = 1,
        ClientCanceled       = 2,
        UserBusy             = 3,
        InvalidForm          = 4,
    };

    uint32_t form_id = 0;
    CloseReason close_reason = CloseReason::ProgrammaticClose;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(form_id);
        stream.writeUnsignedInt(static_cast<uint32_t>(close_reason));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        form_id = stream.readUnsignedInt();
        close_reason = static_cast<CloseReason>(stream.readUnsignedInt());
    }
};

} // namespace cphertunnel
