// CpherTunnel - EduUriResourcePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class EduUriResourcePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(EduUriResourcePacket, EduUriResource)

    std::string button_name;
    std::string link_uri;

    void write(BinaryStream& stream) const override {
        stream.writeString(button_name);
        stream.writeString(link_uri);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        button_name = stream.readString();
        link_uri = stream.readString();
    }
};

} // namespace cphertunnel
