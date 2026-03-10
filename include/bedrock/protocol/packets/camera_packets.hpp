// =============================================================================
// BedrockProtocol - Camera Packets
// Camera presets, instructions, aim assist, shaking, spline
// =============================================================================
#pragma once

#include "../packet.hpp"

namespace bedrock {

// =============================================================================
// CameraShakePacket (ID: 159)
// =============================================================================
class CameraShakePacket : public Packet {
public:
    BEDROCK_PACKET(CameraShakePacket, CameraShake)

    float intensity = 0.0f;
    float seconds = 0.0f;
    uint8_t shake_type = 0;  // 0=positional, 1=rotational
    uint8_t shake_action = 0; // 0=add, 1=stop

    void write(BinaryStream& stream) const override {
        stream.writeFloat(intensity);
        stream.writeFloat(seconds);
        stream.writeByte(shake_type);
        stream.writeByte(shake_action);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        intensity = stream.readFloat();
        seconds = stream.readFloat();
        shake_type = stream.readByte();
        shake_action = stream.readByte();
    }
};

// =============================================================================
// CameraPacket (ID: 73)
// =============================================================================
class CameraPacket : public Packet {
public:
    BEDROCK_PACKET(CameraPacket, Camera)

    ActorUniqueID camera_entity_id;
    ActorUniqueID target_player_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(camera_entity_id);
        stream.writeActorUniqueID(target_player_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        camera_entity_id = stream.readActorUniqueID();
        target_player_id = stream.readActorUniqueID();
    }
};

// =============================================================================
// CameraAimAssistPacket (ID: 316)
// =============================================================================
class CameraAimAssistPacket : public Packet {
public:
    BEDROCK_PACKET(CameraAimAssistPacket, CameraAimAssist)

    std::string preset_id;
    Vec2 view_angle;
    float distance = 0.0f;
    uint8_t target_mode = 0;
    uint8_t action = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(preset_id);
        stream.writeVec2(view_angle);
        stream.writeFloat(distance);
        stream.writeByte(target_mode);
        stream.writeByte(action);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        preset_id = stream.readString();
        view_angle = stream.readVec2();
        distance = stream.readFloat();
        target_mode = stream.readByte();
        action = stream.readByte();
    }
};

// =============================================================================
// CameraAimAssistPresetsPacket (ID: 320)
// =============================================================================
class CameraAimAssistPresetsPacket : public Packet {
public:
    BEDROCK_PACKET(CameraAimAssistPresetsPacket, CameraAimAssistPresets)

    // Presets data - complex structure, stored as raw bytes for forward compatibility
    ByteBuffer presets_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(presets_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        presets_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientCameraAimAssistPacket (ID: 321)
// =============================================================================
class ClientCameraAimAssistPacket : public Packet {
public:
    BEDROCK_PACKET(ClientCameraAimAssistPacket, ClientCameraAimAssist)

    Vec2 view_angle;
    float distance = 0.0f;
    uint8_t target_mode = 0;
    ActorUniqueID target_entity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVec2(view_angle);
        stream.writeFloat(distance);
        stream.writeByte(target_mode);
        stream.writeActorUniqueID(target_entity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        view_angle = stream.readVec2();
        distance = stream.readFloat();
        target_mode = stream.readByte();
        target_entity_id = stream.readActorUniqueID();
    }
};

// =============================================================================
// CameraAimAssistActorPriorityPacket (ID: 339)
// =============================================================================
class CameraAimAssistActorPriorityPacket : public Packet {
public:
    BEDROCK_PACKET(CameraAimAssistActorPriorityPacket, CameraAimAssistActorPriority)

    ActorUniqueID entity_unique_id;
    std::string preset_id;
    std::string category_id;
    std::string actor_id;
    uint8_t priority = 0;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_unique_id);
        stream.writeString(preset_id);
        stream.writeString(category_id);
        stream.writeString(actor_id);
        stream.writeByte(priority);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readActorUniqueID();
        preset_id = stream.readString();
        category_id = stream.readString();
        actor_id = stream.readString();
        priority = stream.readByte();
    }
};

// =============================================================================
// CameraPresetsPacket (ID: 198)
// =============================================================================
class CameraPresetsPacket : public Packet {
public:
    BEDROCK_PACKET(CameraPresetsPacket, CameraPresets)

    // Camera presets are complex nested structures - stored as raw bytes
    ByteBuffer presets_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(presets_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        presets_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// CameraInstructionPacket (ID: 300)
// =============================================================================
class CameraInstructionPacket : public Packet {
public:
    BEDROCK_PACKET(CameraInstructionPacket, CameraInstruction)

    // Camera instructions have many optional sub-instructions (Set, Clear, Fade,
    // Target, FieldOfView, Spline, AttachToEntity, DetachFromEntity).
    // The full structure is complex; we store as raw bytes for forward compat.
    ByteBuffer instruction_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(instruction_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        instruction_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// CameraSplinePacket (ID: 338)
// =============================================================================
class CameraSplinePacket : public Packet {
public:
    BEDROCK_PACKET(CameraSplinePacket, CameraSpline)

    ByteBuffer spline_data; // complex structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(spline_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        spline_data = stream.readRemainingBytes();
    }
};

} // namespace bedrock
