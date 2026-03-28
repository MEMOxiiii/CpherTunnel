// CpherTunnel - CodeBuilderSourcePacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CodeBuilderSourcePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CodeBuilderSourcePacket, CodeBuilderSource)

    uint8_t operation = 0;
    uint8_t category = 0;
    std::string source_code;

    void write(BinaryStream& stream) const override {
        stream.writeByte(operation);
        stream.writeByte(category);
        stream.writeString(source_code);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        operation = stream.readByte();
        category = stream.readByte();
        source_code = stream.readString();
    }
};

} // namespace cphertunnel
