// CpherTunnel - ToastRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ToastRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ToastRequestPacket, ToastRequest)

    std::string title;
    std::string content;

    void write(BinaryStream& stream) const override {
        stream.writeString(title);
        stream.writeString(content);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        title = stream.readString();
        content = stream.readString();
    }
};

} // namespace cphertunnel
