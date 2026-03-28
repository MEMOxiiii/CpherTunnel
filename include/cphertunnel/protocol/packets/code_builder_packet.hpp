// CpherTunnel - CodeBuilderPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CodeBuilderPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CodeBuilderPacket, CodeBuilder)

    std::string url;
    bool should_open = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(url);
        stream.writeBool(should_open);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        url = stream.readString();
        should_open = stream.readBool();
    }
};

} // namespace cphertunnel
