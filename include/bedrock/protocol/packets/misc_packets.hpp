// =============================================================================
// BedrockProtocol - Miscellaneous Packets
// All remaining packets that don't fit other categories
// =============================================================================
#pragma once

#include "../packet.hpp"
#include "../nbt.hpp"

namespace bedrock {

// =============================================================================
// BlockPickRequestPacket (ID: 34)
// =============================================================================
class BlockPickRequestPacket : public Packet {
public:
    BEDROCK_PACKET(BlockPickRequestPacket, BlockPickRequest)

    NetworkBlockPos position;
    bool add_user_data = false;
    uint8_t hotbar_slot = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(position.x);
        stream.writeVarInt(static_cast<int32_t>(position.y));
        stream.writeVarInt(position.z);
        stream.writeBool(add_user_data);
        stream.writeByte(hotbar_slot);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position.x = stream.readVarInt();
        position.y = static_cast<uint32_t>(stream.readVarInt());
        position.z = stream.readVarInt();
        add_user_data = stream.readBool();
        hotbar_slot = stream.readByte();
    }
};

// =============================================================================
// ActorPickRequestPacket (ID: 35)
// =============================================================================
class ActorPickRequestPacket : public Packet {
public:
    BEDROCK_PACKET(ActorPickRequestPacket, ActorPickRequest)

    int64_t entity_id = 0;
    uint8_t hotbar_slot = 0;
    bool with_data = false;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(entity_id);
        stream.writeByte(hotbar_slot);
        stream.writeBool(with_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readSignedInt64();
        hotbar_slot = stream.readByte();
        with_data = stream.readBool();
    }
};

// =============================================================================
// MapInfoRequestPacket (ID: 68)
// =============================================================================
class MapInfoRequestPacket : public Packet {
public:
    BEDROCK_PACKET(MapInfoRequestPacket, MapInfoRequest)

    ActorUniqueID map_id;
    std::vector<uint32_t> client_pixel_request;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(map_id);
        stream.writeUnsignedInt(static_cast<uint32_t>(client_pixel_request.size()));
        for (auto p : client_pixel_request) {
            stream.writeUnsignedInt(p);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        map_id = stream.readActorUniqueID();
        uint32_t count = stream.readUnsignedInt();
        client_pixel_request.resize(count);
        for (auto& p : client_pixel_request) {
            p = stream.readUnsignedInt();
        }
    }
};

// =============================================================================
// MapCreateLockedCopyPacket (ID: 131)
// =============================================================================
class MapCreateLockedCopyPacket : public Packet {
public:
    BEDROCK_PACKET(MapCreateLockedCopyPacket, MapCreateLockedCopy)

    ActorUniqueID original_map_id;
    ActorUniqueID new_map_id;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(original_map_id);
        stream.writeActorUniqueID(new_map_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        original_map_id = stream.readActorUniqueID();
        new_map_id = stream.readActorUniqueID();
    }
};

// =============================================================================
// LecternUpdatePacket (ID: 125)
// =============================================================================
class LecternUpdatePacket : public Packet {
public:
    BEDROCK_PACKET(LecternUpdatePacket, LecternUpdate)

    uint8_t page = 0;
    uint8_t total_pages = 0;
    NetworkBlockPos position;

    void write(BinaryStream& stream) const override {
        stream.writeByte(page);
        stream.writeByte(total_pages);
        stream.writeNetworkBlockPos(position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        page = stream.readByte();
        total_pages = stream.readByte();
        position = stream.readNetworkBlockPos();
    }
};

// =============================================================================
// EducationSettingsPacket (ID: 137)
// =============================================================================
class EducationSettingsPacket : public Packet {
public:
    BEDROCK_PACKET(EducationSettingsPacket, EducationSettings)

    std::string code_builder_uri;
    std::string code_builder_title;
    bool can_resize_code_builder = false;
    bool has_override_uri = false;
    std::string override_uri;
    bool has_quiz = false;
    std::string quiz_url;

    void write(BinaryStream& stream) const override {
        stream.writeString(code_builder_uri);
        stream.writeString(code_builder_title);
        stream.writeBool(can_resize_code_builder);
        stream.writeBool(has_override_uri);
        if (has_override_uri) {
            stream.writeString(override_uri);
        }
        stream.writeBool(has_quiz);
        if (has_quiz) {
            stream.writeString(quiz_url);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        code_builder_uri = stream.readString();
        code_builder_title = stream.readString();
        can_resize_code_builder = stream.readBool();
        has_override_uri = stream.readBool();
        if (has_override_uri) {
            override_uri = stream.readString();
        }
        has_quiz = stream.readBool();
        if (has_quiz) {
            quiz_url = stream.readString();
        }
    }
};

// =============================================================================
// MultiplayerSettingsPacket (ID: 139)
// =============================================================================
class MultiplayerSettingsPacket : public Packet {
public:
    BEDROCK_PACKET(MultiplayerSettingsPacket, MultiplayerSettings)

    int32_t action_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action_type = stream.readVarInt();
    }
};

// =============================================================================
// ClientCacheStatusPacket (ID: 129)
// =============================================================================
class ClientCacheStatusPacket : public Packet {
public:
    BEDROCK_PACKET(ClientCacheStatusPacket, ClientCacheStatus)

    bool cache_supported = false;

    void write(BinaryStream& stream) const override {
        stream.writeBool(cache_supported);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        cache_supported = stream.readBool();
    }
};

// =============================================================================
// SimulationTypePacket (ID: 168)
// =============================================================================
class SimulationTypePacket : public Packet {
public:
    BEDROCK_PACKET(SimulationTypePacket, SimulationType)

    uint8_t simulation_type = 0; // 0=Game, 1=Editor, 2=Test, 3=Invalid

    void write(BinaryStream& stream) const override {
        stream.writeByte(simulation_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        simulation_type = stream.readByte();
    }
};

// =============================================================================
// PhotoTransferPacket (ID: 99)
// =============================================================================
class PhotoTransferPacket : public Packet {
public:
    BEDROCK_PACKET(PhotoTransferPacket, PhotoTransfer)

    std::string photo_name;
    std::string photo_data;
    std::string book_id;
    uint8_t photo_type = 0;
    uint8_t source_type = 0;
    int64_t owner_entity_unique_id = 0;
    std::string new_photo_name;

    void write(BinaryStream& stream) const override {
        stream.writeString(photo_name);
        stream.writeString(photo_data);
        stream.writeString(book_id);
        stream.writeByte(photo_type);
        stream.writeByte(source_type);
        stream.writeSignedInt64(owner_entity_unique_id);
        stream.writeString(new_photo_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        photo_name = stream.readString();
        photo_data = stream.readString();
        book_id = stream.readString();
        photo_type = stream.readByte();
        source_type = stream.readByte();
        owner_entity_unique_id = stream.readSignedInt64();
        new_photo_name = stream.readString();
    }
};

// =============================================================================
// DebugInfoPacket (ID: 155)
// =============================================================================
class DebugInfoPacket : public Packet {
public:
    BEDROCK_PACKET(DebugInfoPacket, DebugInfo)

    ActorUniqueID entity_id;
    std::string data;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(entity_id);
        stream.writeString(data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_id = stream.readActorUniqueID();
        data = stream.readString();
    }
};

// =============================================================================
// PurchaseReceiptPacket (ID: 92)
// =============================================================================
class PurchaseReceiptPacket : public Packet {
public:
    BEDROCK_PACKET(PurchaseReceiptPacket, PurchaseReceipt)

    std::vector<std::string> receipt_ids;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(receipt_ids.size()));
        for (auto& r : receipt_ids) stream.writeString(r);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        receipt_ids.resize(count);
        for (auto& r : receipt_ids) r = stream.readString();
    }
};

// =============================================================================
// AutomationClientConnectPacket (ID: 95)
// =============================================================================
class AutomationClientConnectPacket : public Packet {
public:
    BEDROCK_PACKET(AutomationClientConnectPacket, AutomationClientConnect)

    std::string server_uri;

    void write(BinaryStream& stream) const override {
        stream.writeString(server_uri);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_uri = stream.readString();
    }
};

// =============================================================================
// LabTablePacket (ID: 109)
// =============================================================================
class LabTablePacket : public Packet {
public:
    BEDROCK_PACKET(LabTablePacket, LabTable)

    uint8_t action_type = 0;
    NetworkBlockPos position;
    uint8_t reaction_type = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action_type);
        stream.writeNetworkBlockPos(position);
        stream.writeByte(reaction_type);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action_type = stream.readByte();
        position = stream.readNetworkBlockPos();
        reaction_type = stream.readByte();
    }
};

// =============================================================================
// AddBehaviorTreePacket (ID: 89)
// =============================================================================
class AddBehaviorTreePacket : public Packet {
public:
    BEDROCK_PACKET(AddBehaviorTreePacket, AddBehaviorTree)

    std::string behavior_tree_json;

    void write(BinaryStream& stream) const override {
        stream.writeString(behavior_tree_json);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        behavior_tree_json = stream.readString();
    }
};

// =============================================================================
// EduUriResourcePacket (ID: 170)
// =============================================================================
class EduUriResourcePacket : public Packet {
public:
    BEDROCK_PACKET(EduUriResourcePacket, EduUriResource)

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

// =============================================================================
// CreatePhotoPacket (ID: 171)
// =============================================================================
class CreatePhotoPacket : public Packet {
public:
    BEDROCK_PACKET(CreatePhotoPacket, CreatePhoto)

    int64_t entity_unique_id = 0;
    std::string photo_name;
    std::string item_name;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(entity_unique_id);
        stream.writeString(photo_name);
        stream.writeString(item_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        entity_unique_id = stream.readSignedInt64();
        photo_name = stream.readString();
        item_name = stream.readString();
    }
};

// =============================================================================
// LessonProgressPacket (ID: 183)
// =============================================================================
class LessonProgressPacket : public Packet {
public:
    BEDROCK_PACKET(LessonProgressPacket, LessonProgress)

    int32_t action = 0;
    int32_t score = 0;
    std::string activity_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(action);
        stream.writeVarInt(score);
        stream.writeString(activity_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readVarInt();
        score = stream.readVarInt();
        activity_id = stream.readString();
    }
};

// =============================================================================
// CodeBuilderPacket (ID: 150)
// =============================================================================
class CodeBuilderPacket : public Packet {
public:
    BEDROCK_PACKET(CodeBuilderPacket, CodeBuilder)

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

// =============================================================================
// CodeBuilderSourcePacket (ID: 178)
// =============================================================================
class CodeBuilderSourcePacket : public Packet {
public:
    BEDROCK_PACKET(CodeBuilderSourcePacket, CodeBuilderSource)

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

// =============================================================================
// TickingAreasLoadStatusPacket (ID: 179)
// =============================================================================
class TickingAreasLoadStatusPacket : public Packet {
public:
    BEDROCK_PACKET(TickingAreasLoadStatusPacket, TickingAreasLoadStatus)

    bool waiting_for_preload = false;

    void write(BinaryStream& stream) const override {
        stream.writeBool(waiting_for_preload);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        waiting_for_preload = stream.readBool();
    }
};

// =============================================================================
// ServerboundLoadingScreenPacket (ID: 312)
// =============================================================================
class ServerboundLoadingScreenPacket : public Packet {
public:
    BEDROCK_PACKET(ServerboundLoadingScreenPacket, ServerboundLoadingScreen)

    int32_t type = 0;
    std::optional<uint32_t> loading_screen_id;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(type);
        stream.writeBool(loading_screen_id.has_value());
        if (loading_screen_id.has_value()) {
            stream.writeUnsignedInt(loading_screen_id.value());
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        type = stream.readVarInt();
        if (stream.readBool()) {
            loading_screen_id = stream.readUnsignedInt();
        }
    }
};

// =============================================================================
// ServerboundDiagnosticsPacket (ID: 315)
// =============================================================================
class ServerboundDiagnosticsPacket : public Packet {
public:
    BEDROCK_PACKET(ServerboundDiagnosticsPacket, ServerboundDiagnostics)

    float avg_fps = 0.0f;
    float avg_server_sim_tick_time = 0.0f;
    float avg_client_sim_tick_time = 0.0f;
    float avg_begin_frame_time = 0.0f;
    float avg_input_time = 0.0f;
    float avg_render_time = 0.0f;
    float avg_end_frame_time = 0.0f;
    float avg_remaining_time_percent = 0.0f;
    float avg_unaccounted_time_percent = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeFloat(avg_fps);
        stream.writeFloat(avg_server_sim_tick_time);
        stream.writeFloat(avg_client_sim_tick_time);
        stream.writeFloat(avg_begin_frame_time);
        stream.writeFloat(avg_input_time);
        stream.writeFloat(avg_render_time);
        stream.writeFloat(avg_end_frame_time);
        stream.writeFloat(avg_remaining_time_percent);
        stream.writeFloat(avg_unaccounted_time_percent);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        avg_fps = stream.readFloat();
        avg_server_sim_tick_time = stream.readFloat();
        avg_client_sim_tick_time = stream.readFloat();
        avg_begin_frame_time = stream.readFloat();
        avg_input_time = stream.readFloat();
        avg_render_time = stream.readFloat();
        avg_end_frame_time = stream.readFloat();
        avg_remaining_time_percent = stream.readFloat();
        avg_unaccounted_time_percent = stream.readFloat();
    }
};

// =============================================================================
// RefreshEntitlementsPacket (ID: 305)
// =============================================================================
class RefreshEntitlementsPacket : public Packet {
public:
    BEDROCK_PACKET(RefreshEntitlementsPacket, RefreshEntitlements)

    void write(BinaryStream&) const override {}
    void read(ReadOnlyBinaryStream&) override {}
};

// =============================================================================
// ServerStatsPacket (ID: 192)
// =============================================================================
class ServerStatsPacket : public Packet {
public:
    BEDROCK_PACKET(ServerStatsPacket, ServerStats)

    float server_time = 0.0f;
    float network_time = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeFloat(server_time);
        stream.writeFloat(network_time);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        server_time = stream.readFloat();
        network_time = stream.readFloat();
    }
};

// =============================================================================
// UpdateClientInputLocksPacket (ID: 196)
// =============================================================================
class UpdateClientInputLocksPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateClientInputLocksPacket, PlayerClientInputPermissions)

    uint32_t lock_component_data = 0;
    Vec3 server_position;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(lock_component_data);
        stream.writeVec3(server_position);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        lock_component_data = stream.readUnsignedVarInt();
        server_position = stream.readVec3();
    }
};

// =============================================================================
// UpdateClientOptionsPacket (ID: 323)
// =============================================================================
class UpdateClientOptionsPacket : public Packet {
public:
    BEDROCK_PACKET(UpdateClientOptionsPacket, UpdateClientOptions)

    ByteBuffer options_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(options_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        options_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientboundControlSchemeSetPacket (ID: 327)
// =============================================================================
class ClientboundControlSchemeSetPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundControlSchemeSetPacket, ClientboundControlSchemeSet)

    uint8_t control_scheme = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(control_scheme);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        control_scheme = stream.readByte();
    }
};

// =============================================================================
// PlayerFogPacket (ID: 160)
// =============================================================================
class PlayerFogPacket : public Packet {
public:
    BEDROCK_PACKET(PlayerFogPacket, PlayerFogSetting)

    std::vector<std::string> fog_stack;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(fog_stack.size()));
        for (auto& f : fog_stack) stream.writeString(f);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        fog_stack.resize(count);
        for (auto& f : fog_stack) f = stream.readString();
    }
};

// =============================================================================
// OpenSignPacket (ID: 303)
// =============================================================================
class OpenSignPacket : public Packet {
public:
    BEDROCK_PACKET(OpenSignPacket, OpenSign)

    NetworkBlockPos position;
    bool front_side = true;

    void write(BinaryStream& stream) const override {
        stream.writeNetworkBlockPos(position);
        stream.writeBool(front_side);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        position = stream.readNetworkBlockPos();
        front_side = stream.readBool();
    }
};

// =============================================================================
// EditorNetworkPacket (ID: 190)
// =============================================================================
class EditorNetworkPacket : public Packet {
public:
    BEDROCK_PACKET(EditorNetworkPacket, EditorNetwork)

    std::shared_ptr<NbtTag> editor_data;

    void write(BinaryStream& stream) const override {
        if (editor_data) {
            NbtTag::writeNetworkNbt(stream, *editor_data);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        editor_data = NbtTag::readNetworkNbt(stream);
    }
};

// =============================================================================
// UnlockedRecipesPacket (ID: 199)
// =============================================================================
class UnlockedRecipesPacket : public Packet {
public:
    BEDROCK_PACKET(UnlockedRecipesPacket, UnlockedRecipes)

    uint32_t unlock_type = 0;
    std::vector<std::string> recipes;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(unlock_type);
        stream.writeUnsignedVarInt(static_cast<uint32_t>(recipes.size()));
        for (auto& r : recipes) stream.writeString(r);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        unlock_type = stream.readUnsignedInt();
        uint32_t count = stream.readUnsignedVarInt();
        recipes.resize(count);
        for (auto& r : recipes) r = stream.readString();
    }
};

// =============================================================================
// PlayerVideoCapturePacket (ID: 324)
// =============================================================================
class PlayerVideoCapturePacket : public Packet {
public:
    BEDROCK_PACKET(PlayerVideoCapturePacket, PlayerVideoCapture)

    uint8_t action = 0;
    ByteBuffer capture_data;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeRawBytes(capture_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        capture_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// VoxelShapesPacket (ID: 337)
// =============================================================================
class VoxelShapesPacket : public Packet {
public:
    BEDROCK_PACKET(VoxelShapesPacket, VoxelShapes)

    ByteBuffer shapes_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shapes_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shapes_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientboundTextureShiftPacket (ID: 336)
// =============================================================================
class ClientboundTextureShiftPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundTextureShiftPacket, ClientboundTextureShift)

    ByteBuffer shift_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shift_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shift_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientboundDataStorePacket (ID: 330)
// =============================================================================
class ClientboundDataStorePacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundDataStorePacket, ClientboundDataStore)

    ByteBuffer store_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(store_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        store_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ServerboundDataStorePacket (ID: 332)
// =============================================================================
class ServerboundDataStorePacket : public Packet {
public:
    BEDROCK_PACKET(ServerboundDataStorePacket, ServerboundDataStore)

    ByteBuffer store_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(store_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        store_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientboundDebugRendererPacket (ID: 164)
// =============================================================================
class ClientboundDebugRendererPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundDebugRendererPacket, ClientBoundDebugRenderer)

    enum class Type : uint32_t {
        ClearDebugMarkers = 1,
        AddDebugMarkerCube = 2,
        ClearAllDebugRenderers = 3,
    };

    Type type = Type::ClearAllDebugRenderers;
    // Fields for AddDebugMarkerCube:
    std::string text;
    Vec3 position;
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
    int64_t duration_ms = 0;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedInt(static_cast<uint32_t>(type));
        if (type == Type::AddDebugMarkerCube) {
            stream.writeString(text);
            stream.writeVec3(position);
            stream.writeFloat(r);
            stream.writeFloat(g);
            stream.writeFloat(b);
            stream.writeFloat(a);
            stream.writeSignedInt64(duration_ms);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        type = static_cast<Type>(stream.readUnsignedInt());
        if (type == Type::AddDebugMarkerCube) {
            text = stream.readString();
            position = stream.readVec3();
            r = stream.readFloat();
            g = stream.readFloat();
            b = stream.readFloat();
            a = stream.readFloat();
            duration_ms = stream.readSignedInt64();
        }
    }
};

// =============================================================================
// CurrentStructureFeaturePacket (ID: 314)
// =============================================================================
class CurrentStructureFeaturePacket : public Packet {
public:
    BEDROCK_PACKET(CurrentStructureFeaturePacket, CurrentStructureFeature)

    std::string current_structure_feature;

    void write(BinaryStream& stream) const override {
        stream.writeString(current_structure_feature);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        current_structure_feature = stream.readString();
    }
};

// =============================================================================
// DebugDrawerPacket (ID: 328)
// =============================================================================
class DebugDrawerPacket : public Packet {
public:
    BEDROCK_PACKET(DebugDrawerPacket, ServerScriptDebugDrawer)

    ByteBuffer shape_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(shape_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        shape_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// FeatureRegistryPacket (ID: 191)
// =============================================================================
class FeatureRegistryPacket : public Packet {
public:
    BEDROCK_PACKET(FeatureRegistryPacket, FeatureRegistry)

    struct FeatureEntry {
        std::string feature_name;
        std::string binary_json_output;
    };

    std::vector<FeatureEntry> features;

    void write(BinaryStream& stream) const override {
        stream.writeUnsignedVarInt(static_cast<uint32_t>(features.size()));
        for (const auto& f : features) {
            stream.writeString(f.feature_name);
            stream.writeString(f.binary_json_output);
        }
    }

    void read(ReadOnlyBinaryStream& stream) override {
        uint32_t count = stream.readUnsignedVarInt();
        features.resize(count);
        for (auto& f : features) {
            f.feature_name = stream.readString();
            f.binary_json_output = stream.readString();
        }
    }
};

// =============================================================================
// GameTestRequestPacket (ID: 194)
// =============================================================================
class GameTestRequestPacket : public Packet {
public:
    BEDROCK_PACKET(GameTestRequestPacket, GameTestRequest)

    int32_t max_tests_per_batch = 0;
    int32_t repeat_count = 0;
    uint8_t rotation = 0;
    bool stop_on_failure = false;
    BlockPos test_pos;
    int32_t tests_per_row = 0;
    std::string test_name;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(max_tests_per_batch);
        stream.writeVarInt(repeat_count);
        stream.writeByte(rotation);
        stream.writeBool(stop_on_failure);
        stream.writeBlockPos(test_pos);
        stream.writeVarInt(tests_per_row);
        stream.writeString(test_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        max_tests_per_batch = stream.readVarInt();
        repeat_count = stream.readVarInt();
        rotation = stream.readByte();
        stop_on_failure = stream.readBool();
        test_pos = stream.readBlockPos();
        tests_per_row = stream.readVarInt();
        test_name = stream.readString();
    }
};

// =============================================================================
// GameTestResultsPacket (ID: 195)
// =============================================================================
class GameTestResultsPacket : public Packet {
public:
    BEDROCK_PACKET(GameTestResultsPacket, GameTestResults)

    bool succeeded = false;
    std::string error;
    std::string test_name;

    void write(BinaryStream& stream) const override {
        stream.writeBool(succeeded);
        stream.writeString(error);
        stream.writeString(test_name);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        succeeded = stream.readBool();
        error = stream.readString();
        test_name = stream.readString();
    }
};

// =============================================================================
// GraphicsOverrideParameterPacket (ID: 331)
// =============================================================================
class GraphicsOverrideParameterPacket : public Packet {
public:
    BEDROCK_PACKET(GraphicsOverrideParameterPacket, GraphicsParameterOverride)

    ByteBuffer parameter_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(parameter_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        parameter_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ItemRegistryPacket (ID: 162)
// =============================================================================
class ItemRegistryPacket : public Packet {
public:
    BEDROCK_PACKET(ItemRegistryPacket, ItemRegistry)

    // Item registry is a complex structure - stored as raw bytes
    ByteBuffer items_data;

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(items_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        items_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// JigsawStructureDataPacket (ID: 313)
// =============================================================================
class JigsawStructureDataPacket : public Packet {
public:
    BEDROCK_PACKET(JigsawStructureDataPacket, JigsawStructureData)

    ByteBuffer structure_data_nbt; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(structure_data_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_data_nbt = stream.readRemainingBytes();
    }
};

// =============================================================================
// LegacyTelemetryEventPacket (ID: 65)
// =============================================================================
class LegacyTelemetryEventPacket : public Packet {
public:
    BEDROCK_PACKET(LegacyTelemetryEventPacket, LegacyTelemetryEvent)

    ActorUniqueID target_actor_id;
    int32_t event_type = 0;
    bool use_player_id = false;
    // Event data varies by event_type; stored as raw bytes for the variant payload
    ByteBuffer event_data;

    void write(BinaryStream& stream) const override {
        stream.writeActorUniqueID(target_actor_id);
        stream.writeVarInt(event_type);
        stream.writeBool(use_player_id);
        stream.writeRawBytes(event_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        target_actor_id = stream.readActorUniqueID();
        event_type = stream.readVarInt();
        use_player_id = stream.readBool();
        event_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// LocatorBarPacket (ID: 341)
// =============================================================================
class LocatorBarPacket : public Packet {
public:
    BEDROCK_PACKET(LocatorBarPacket, LocatorBar)

    ByteBuffer waypoints_data; // array of LocatorBarWaypointPayload

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(waypoints_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        waypoints_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// PartyChangedPacket (ID: 342)
// =============================================================================
class PartyChangedPacket : public Packet {
public:
    BEDROCK_PACKET(PartyChangedPacket, PartyChanged)

    std::string party_id;

    void write(BinaryStream& stream) const override {
        stream.writeString(party_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        party_id = stream.readString();
    }
};

// =============================================================================
// PositionTrackingDBClientRequestPacket (ID: 154)
// =============================================================================
class PositionTrackingDBClientRequestPacket : public Packet {
public:
    BEDROCK_PACKET(PositionTrackingDBClientRequestPacket, PositionTrackingDBClientRequest)

    uint8_t action = 0; // 0=Query
    int32_t tracking_id = 0;

    void write(BinaryStream& stream) const override {
        stream.writeByte(action);
        stream.writeVarInt(tracking_id);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = stream.readByte();
        tracking_id = stream.readVarInt();
    }
};

// =============================================================================
// PositionTrackingDBServerBroadcastPacket (ID: 153)
// =============================================================================
class PositionTrackingDBServerBroadcastPacket : public Packet {
public:
    BEDROCK_PACKET(PositionTrackingDBServerBroadcastPacket, PositionTrackingDBServerBroadcast)

    enum class Action : uint8_t {
        Update   = 0,
        Destroy  = 1,
        NotFound = 2,
    };

    Action action = Action::Update;
    int32_t tracking_id = 0;
    ByteBuffer position_data_nbt; // CompoundTag

    void write(BinaryStream& stream) const override {
        stream.writeByte(static_cast<uint8_t>(action));
        stream.writeVarInt(tracking_id);
        stream.writeRawBytes(position_data_nbt);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        action = static_cast<Action>(stream.readByte());
        tracking_id = stream.readVarInt();
        position_data_nbt = stream.readRemainingBytes();
    }
};

// =============================================================================
// RequestAbilityPacket (ID: 184)
// =============================================================================
class RequestAbilityPacket : public Packet {
public:
    BEDROCK_PACKET(RequestAbilityPacket, RequestAbility)

    enum class ValueType : uint8_t {
        Unset = 0,
        Bool  = 1,
        Float = 2,
    };

    int32_t ability = 0;
    ValueType value_type = ValueType::Unset;
    bool bool_value = false;
    float float_value = 0.0f;

    void write(BinaryStream& stream) const override {
        stream.writeVarInt(ability);
        stream.writeByte(static_cast<uint8_t>(value_type));
        stream.writeBool(bool_value);
        stream.writeFloat(float_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        ability = stream.readVarInt();
        value_type = static_cast<ValueType>(stream.readByte());
        bool_value = stream.readBool();
        float_value = stream.readFloat();
    }
};

// =============================================================================
// RequestPermissionsPacket (ID: 185)
// =============================================================================
class RequestPermissionsPacket : public Packet {
public:
    BEDROCK_PACKET(RequestPermissionsPacket, RequestPermissions)

    int64_t target_player_raw_id = 0;
    int32_t player_permission_level = 0;
    uint16_t custom_permission_flags = 0;

    void write(BinaryStream& stream) const override {
        stream.writeSignedInt64(target_player_raw_id);
        stream.writeVarInt(player_permission_level);
        stream.writeUnsignedShort(custom_permission_flags);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        target_player_raw_id = stream.readSignedInt64();
        player_permission_level = stream.readVarInt();
        custom_permission_flags = stream.readUnsignedShort();
    }
};

// =============================================================================
// ServerboundDataDrivenScreenClosedPacket (ID: 343)
// =============================================================================
class ServerboundDataDrivenScreenClosedPacket : public Packet {
public:
    BEDROCK_PACKET(ServerboundDataDrivenScreenClosedPacket, ServerboundDataDrivenScreenClosed)

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

// =============================================================================
// ServerboundPackSettingChangePacket (ID: 329)
// =============================================================================
class ServerboundPackSettingChangePacket : public Packet {
public:
    BEDROCK_PACKET(ServerboundPackSettingChangePacket, ServerboundPackSettingChange)

    bedrock::UUID pack_id;
    std::string pack_setting_name;
    // value can be float, boolean, or string — stored as variant bytes
    ByteBuffer setting_value;

    void write(BinaryStream& stream) const override {
        stream.writeUUID(pack_id);
        stream.writeString(pack_setting_name);
        stream.writeRawBytes(setting_value);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        pack_id = stream.readUUID();
        pack_setting_name = stream.readString();
        setting_value = stream.readRemainingBytes();
    }
};

// =============================================================================
// StructureTemplateDataRequestPacket (ID: 132)
// =============================================================================
class StructureTemplateDataRequestPacket : public Packet {
public:
    BEDROCK_PACKET(StructureTemplateDataRequestPacket, StructureTemplateDataExportRequest)

    std::string structure_name;
    BlockPos position;
    // Structure Settings
    std::string palette_name;
    bool ignore_entities = false;
    bool ignore_blocks = false;
    bool allow_non_ticking_chunks = false;
    BlockPos structure_size;
    BlockPos structure_offset;
    ActorUniqueID last_edit_player;
    uint8_t rotation = 0;
    uint8_t mirror = 0;
    uint8_t animation_mode = 0;
    float animation_seconds = 0.0f;
    float integrity_value = 100.0f;
    uint32_t integrity_seed = 0;
    Vec3 rotation_pivot;
    uint8_t requested_operation = 0;

    void write(BinaryStream& stream) const override {
        stream.writeString(structure_name);
        stream.writeBlockPos(position);
        // Settings
        stream.writeString(palette_name);
        stream.writeBool(ignore_entities);
        stream.writeBool(ignore_blocks);
        stream.writeBool(allow_non_ticking_chunks);
        stream.writeBlockPos(structure_size);
        stream.writeBlockPos(structure_offset);
        stream.writeActorUniqueID(last_edit_player);
        stream.writeByte(rotation);
        stream.writeByte(mirror);
        stream.writeByte(animation_mode);
        stream.writeFloat(animation_seconds);
        stream.writeFloat(integrity_value);
        stream.writeUnsignedInt(integrity_seed);
        stream.writeVec3(rotation_pivot);
        stream.writeByte(requested_operation);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_name = stream.readString();
        position = stream.readBlockPos();
        palette_name = stream.readString();
        ignore_entities = stream.readBool();
        ignore_blocks = stream.readBool();
        allow_non_ticking_chunks = stream.readBool();
        structure_size = stream.readBlockPos();
        structure_offset = stream.readBlockPos();
        last_edit_player = stream.readActorUniqueID();
        rotation = stream.readByte();
        mirror = stream.readByte();
        animation_mode = stream.readByte();
        animation_seconds = stream.readFloat();
        integrity_value = stream.readFloat();
        integrity_seed = stream.readUnsignedInt();
        rotation_pivot = stream.readVec3();
        requested_operation = stream.readByte();
    }
};

// =============================================================================
// StructureTemplateDataResponsePacket (ID: 133)
// =============================================================================
class StructureTemplateDataResponsePacket : public Packet {
public:
    BEDROCK_PACKET(StructureTemplateDataResponsePacket, StructureTemplateDataExportResponse)

    enum class ResponseType : uint8_t {
        None   = 0,
        Export = 1,
        Query  = 2,
    };

    std::string structure_name;
    ByteBuffer structure_nbt; // optional CompoundTag
    ResponseType response_type = ResponseType::None;

    void write(BinaryStream& stream) const override {
        stream.writeString(structure_name);
        stream.writeRawBytes(structure_nbt);
        stream.writeByte(static_cast<uint8_t>(response_type));
    }

    void read(ReadOnlyBinaryStream& stream) override {
        structure_name = stream.readString();
        // NBT data + response_type at end - simplification: read all remaining
        structure_nbt = stream.readRemainingBytes();
        if (!structure_nbt.empty()) {
            response_type = static_cast<ResponseType>(structure_nbt.back());
            structure_nbt.pop_back();
        }
    }
};

// =============================================================================
// SyncWorldClocksPacket (ID: 344)
// =============================================================================
class SyncWorldClocksPacket : public Packet {
public:
    BEDROCK_PACKET(SyncWorldClocksPacket, SyncWorldClocks)

    ByteBuffer clock_data; // Complex oneOf variant structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(clock_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        clock_data = stream.readRemainingBytes();
    }
};

// =============================================================================
// ClientboundAttributeLayerSyncPacket (ID: 345)
// =============================================================================
class ClientboundAttributeLayerSyncPacket : public Packet {
public:
    BEDROCK_PACKET(ClientboundAttributeLayerSyncPacket, ClientboundAttributeLayerSync)

    ByteBuffer attribute_data; // Complex oneOf variant structure

    void write(BinaryStream& stream) const override {
        stream.writeRawBytes(attribute_data);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        attribute_data = stream.readRemainingBytes();
    }
};

} // namespace bedrock
