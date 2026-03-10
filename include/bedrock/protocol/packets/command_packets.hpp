// =============================================================================
// BedrockProtocol - Command Packets
// Command execution, output, available commands, scoreboard
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// CommandRequestPacket (ID: 77)
// =============================================================================
class CommandRequestPacket : public Packet {
public:
    BEDROCK_PACKET(CommandRequestPacket, CommandRequest)

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

// =============================================================================
// CommandOutputPacket (ID: 79)
// =============================================================================
struct CommandOutputMessage {
    bool is_success = false;
    std::string message_id;
    std::vector<std::string> parameters;
};

class CommandOutputPacket : public Packet {
public:
    BEDROCK_PACKET(CommandOutputPacket, CommandOutput)

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

// =============================================================================
// SettingsCommandPacket (ID: 140)
// =============================================================================
class SettingsCommandPacket : public Packet {
public:
    BEDROCK_PACKET(SettingsCommandPacket, SettingsCommand)

    std::string command;
    bool suppress_output = false;

    void write(BinaryStream& stream) const override {
        stream.writeString(command);
        stream.writeBool(suppress_output);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        command = stream.readString();
        suppress_output = stream.readBool();
    }
};

// =============================================================================
// RemoveObjectivePacket (ID: 106)
// =============================================================================
class RemoveObjectivePacket : public Packet {
public:
    BEDROCK_PACKET(RemoveObjectivePacket, RemoveObjective)

    std::string objective_name;

    void write(BinaryStream& stream) const override {
        stream.writeString(objective_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        objective_name = stream.readString();
    }
};

// =============================================================================
// SetDisplayObjectivePacket (ID: 107)
// =============================================================================
class SetDisplayObjectivePacket : public Packet {
public:
    BEDROCK_PACKET(SetDisplayObjectivePacket, SetDisplayObjective)

    std::string display_slot;
    std::string objective_name;
    std::string display_name;
    std::string criteria_name;
    int32_t sort_order = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(display_slot);
        stream.writeString(objective_name);
        stream.writeString(display_name);
        stream.writeString(criteria_name);
        stream.writeVarInt(sort_order);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        display_slot = stream.readString();
        objective_name = stream.readString();
        display_name = stream.readString();
        criteria_name = stream.readString();
        sort_order = stream.readVarInt();
    }
};

// =============================================================================
// UpdateSoftEnumPacket (ID: 114)
// =============================================================================
enum class SoftEnumUpdateType : uint8_t {
    Add     = 0,
    Remove  = 1,
    Replace = 2
};

class UpdateSoftEnumPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateSoftEnumPacket, UpdateSoftEnum)

    std::string enum_name;
    std::vector<std::string> values;
    SoftEnumUpdateType update_type = SoftEnumUpdateType::Replace;

    void write(BinaryStream& stream) const override {
        stream.writeString(enum_name);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(values.size()));
        for (auto& v : values) stream.writeString(v);
        stream.writeByte(static_cast<uint8_t>(update_type));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        enum_name = stream.readString();
        uint32_t count = stream.readUnsignedVarInt();
        values.resize(count);
        for (auto& v : values) v = stream.readString();
        update_type = static_cast<SoftEnumUpdateType>(stream.readByte());
    }
};

// =============================================================================
// ScriptMessagePacket (ID: 177)
// =============================================================================
class ScriptMessagePacket : public Packet {
public:
    BEDROCK_PACKET(ScriptMessagePacket, ScriptMessage)

    std::string message_id;
    std::string message_value;

    void write(BinaryStream& stream) const override {
        stream.writeString(message_id);
        stream.writeString(message_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        message_id = stream.readString();
        message_value = stream.readString();
    }
};

// =============================================================================
// AvailableCommandsPacket (ID: 76)
// =============================================================================
class AvailableCommandsPacket : public Packet {
public:
    BEDROCK_PACKET(AvailableCommandsPacket, AvailableCommands)

    // The AvailableCommands packet has one of the most complex serialization
    // formats in the protocol, with interlinked enum/command/postfix tables.
    // We store the raw payload for maximum forward compatibility.
    ByteBuffer commands_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(commands_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        commands_data = stream.readRemainingBytes();
    }
};

} // namespace bedrock
