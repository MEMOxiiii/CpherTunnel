// CpherTunnel - SetDifficultyPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class SetDifficultyPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(SetDifficultyPacket, SetDifficulty)

    uint32_t difficulty = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(difficulty);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        difficulty = stream.readUnsignedVarInt();
    }
};

} // namespace cphertunnel
