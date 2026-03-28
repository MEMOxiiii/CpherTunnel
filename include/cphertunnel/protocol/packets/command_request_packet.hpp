// CpherTunnel - CommandRequestPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class CommandRequestPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CommandRequestPacket, CommandRequest)

    std::string command;
    uint32_t command_origin_type = 0; // 0=Player, 1=CommandBlock, 3=DevConsole, etc.
    UUID command_origin_uuid;
    std::string request_id;
    bool is_internal = false;
    int32_t version = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(command);
        // Command origin data
        stream.writeUnsignedVarInt(command_origin_type);
        stream.writeUUID(command_origin_uuid);
        stream.writeString(request_id);
        if (command_origin_type == 3 || command_origin_type == 7) {
            stream.writeVarInt64(0); // player entity id
        }
        stream.writeBool(is_internal);
        stream.writeVarInt(version);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        command = stream.readString();
        command_origin_type = stream.readUnsignedVarInt();
        command_origin_uuid = stream.readUUID();
        request_id = stream.readString();
        if (command_origin_type == 3 || command_origin_type == 7) {
            stream.readVarInt64();
        }
        is_internal = stream.readBool();
        version = stream.readVarInt();
    }
};

} // namespace cphertunnel
