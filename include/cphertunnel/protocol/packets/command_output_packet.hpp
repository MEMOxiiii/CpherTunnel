// CpherTunnel - CommandOutputPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


struct CommandOutputMessage {
    bool is_success = false;
    std::string message_id;
    std::vector<std::string> parameters;
};

class CommandOutputPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CommandOutputPacket, CommandOutput)

    // Command origin
    uint32_t command_origin_type = 0;
    UUID command_origin_uuid;
    std::string request_id;

    uint8_t output_type = 0;
    uint32_t success_count = 0;
    std::vector<CommandOutputMessage> messages;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(command_origin_type);
        stream.writeUUID(command_origin_uuid);
        stream.writeString(request_id);
        stream.writeByte(output_type);
        stream.writeUnsignedVarInt(success_count);

        stream.writeUnsignedVarInt(static_cast<uint32_t>(messages.size()));
        for (auto& msg : messages) {
            stream.writeBool(msg.is_success);
            stream.writeString(msg.message_id);
            stream.writeUnsignedVarInt(static_cast<uint32_t>(msg.parameters.size()));
            for (auto& p : msg.parameters) stream.writeString(p);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        command_origin_type = stream.readUnsignedVarInt();
        command_origin_uuid = stream.readUUID();
        request_id = stream.readString();
        output_type = stream.readByte();
        success_count = stream.readUnsignedVarInt();

        uint32_t msgCount = stream.readUnsignedVarInt();
        messages.resize(msgCount);
        for (auto& msg : messages) {
            msg.is_success = stream.readBool();
            msg.message_id = stream.readString();
            uint32_t pCount = stream.readUnsignedVarInt();
            msg.parameters.resize(pCount);
            for (auto& p : msg.parameters) p = stream.readString();
        }
    }
};

} // namespace cphertunnel
