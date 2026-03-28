// =============================================================================
// CpherTunnel - Packet IDs
// Complete MinecraftPacketIds enum for Protocol 944 (Bedrock 1.26.1)
// =============================================================================
#pragma once

#include <cstdint>

namespace cphertunnel {

enum class PacketId : uint32_t {
    KeepAlive                           = 0,
    Login                               = 1,
    PlayStatus                          = 2,
    ServerToClientHandshake             = 3,
    ClientToServerHandshake             = 4,
    Disconnect                          = 5,
    ResourcePacksInfo                   = 6,
    ResourcePackStack                   = 7,
    ResourcePackClientResponse          = 8,
    Text                                = 9,
    SetTime                             = 10,
    StartGame                           = 11,
    AddPlayer                           = 12,
    AddActor                            = 13,
    RemoveActor                         = 14,
    AddItemActor                        = 15,
    ServerPlayerPostMovePosition        = 16,
    TakeItemActor                       = 17,
    MoveAbsoluteActor                   = 18,
    MovePlayer                          = 19,
    PassengerJump_Deprecated            = 20,
    UpdateBlock                         = 21,
    AddPainting                         = 22,
    TickSync_Deprecated                 = 23,
    LevelSoundEventV1_Deprecated        = 24,
    LevelEvent                          = 25,
    BlockEvent                          = 26,
    ActorEvent                          = 27,
    MobEffect                           = 28,
    UpdateAttributes                    = 29,
    InventoryTransaction                = 30,
    PlayerEquipment                     = 31,
    MobArmorEquipment                   = 32,
    Interact                            = 33,
    BlockPickRequest                    = 34,
    ActorPickRequest                    = 35,
    PlayerAction                        = 36,
    ActorFall_Deprecated                = 37,
    HurtArmor                           = 38,
    SetActorData                        = 39,
    SetActorMotion                      = 40,
    SetActorLink                        = 41,
    SetHealth                           = 42,
    SetSpawnPosition                    = 43,
    Animate                             = 44,
    Respawn                             = 45,
    ContainerOpen                       = 46,
    ContainerClose                      = 47,
    PlayerHotbar                        = 48,
    InventoryContent                    = 49,
    InventorySlot                       = 50,
    ContainerSetData                    = 51,
    CraftingData                        = 52,
    CraftingEvent_Deprecated            = 53,
    GuiDataPickItem                     = 54,
    AdventureSettings_Deprecated        = 55,
    BlockActorData                      = 56,
    PlayerInput_Deprecated              = 57,
    FullChunkData                       = 58,
    SetCommandsEnabled                  = 59,
    SetDifficulty                       = 60,
    ChangeDimension                     = 61,
    SetPlayerGameType                   = 62,
    PlayerList                          = 63,
    SimpleEvent                         = 64,
    LegacyTelemetryEvent                = 65,
    SpawnExperienceOrb                  = 66,
    MapData                             = 67,
    MapInfoRequest                      = 68,
    RequestChunkRadius                  = 69,
    ChunkRadiusUpdated                  = 70,
    ItemFrameDropItem_Deprecated        = 71,
    GameRulesChanged                    = 72,
    Camera                              = 73,
    BossEvent                           = 74,
    ShowCredits                         = 75,
    AvailableCommands                   = 76,
    CommandRequest                      = 77,
    CommandBlockUpdate                  = 78,
    CommandOutput                       = 79,
    UpdateTrade                         = 80,
    UpdateEquip                         = 81,
    ResourcePackDataInfo                = 82,
    ResourcePackChunkData               = 83,
    ResourcePackChunkRequest            = 84,
    Transfer                            = 85,
    PlaySound                           = 86,
    StopSound                           = 87,
    SetTitle                            = 88,
    AddBehaviorTree                     = 89,
    StructureBlockUpdate                = 90,
    ShowStoreOffer                      = 91,
    PurchaseReceipt                     = 92,
    PlayerSkin                          = 93,
    SubClientLogin                      = 94,
    AutomationClientConnect             = 95,
    SetLastHurtBy                       = 96,
    BookEdit                            = 97,
    NpcRequest                          = 98,
    PhotoTransfer                       = 99,
    ModalFormRequest                    = 100,
    ModalFormResponse                   = 101,
    ServerSettingsRequest               = 102,
    ServerSettingsResponse              = 103,
    ShowProfile                         = 104,
    SetDefaultGameType                  = 105,
    RemoveObjective                     = 106,
    SetDisplayObjective                 = 107,
    SetScore                            = 108,
    LabTable                            = 109,
    UpdateBlockSynced                   = 110,
    MoveDeltaActor                      = 111,
    SetScoreboardIdentity               = 112,
    SetLocalPlayerAsInit                = 113,
    UpdateSoftEnum                      = 114,
    Ping                                = 115,
    BlockPalette_Deprecated             = 116,
    ScriptCustomEvent                   = 117,
    SpawnParticleEffect                 = 118,
    AvailableActorIdentifiers           = 119,
    LevelSoundEventV2_Deprecated        = 120,
    NetworkChunkPublisherUpdate         = 121,
    BiomeDefinitionList                 = 122,
    LevelSoundEvent                     = 123,
    LevelEventGeneric                   = 124,
    LecternUpdate                       = 125,
    // 126-128 deprecated
    ClientCacheStatus                   = 129,
    OnScreenTextureAnimation            = 130,
    MapCreateLockedCopy                 = 131,
    StructureTemplateDataExportRequest  = 132,
    StructureTemplateDataExportResponse = 133,
    // 134 reserved
    ClientCacheBlobStatus               = 135,
    ClientCacheMissResponse             = 136,
    EducationSettings                   = 137,
    Emote                               = 138,
    MultiplayerSettings                 = 139,
    SettingsCommand                     = 140,
    AnvilDamage                         = 141,
    CompletedUsingItem                  = 142,
    NetworkSettings                     = 143,
    PlayerAuthInput                     = 144,
    CreativeContent                     = 145,
    PlayerEnchantOptions                = 146,
    ItemStackRequest                    = 147,
    ItemStackResponse                   = 148,
    PlayerArmorDamage                   = 149,
    CodeBuilder                         = 150,
    UpdatePlayerGameType                = 151,
    EmoteList                           = 152,
    PositionTrackingDBServerBroadcast   = 153,
    PositionTrackingDBClientRequest     = 154,
    DebugInfo                           = 155,
    PacketViolationWarning              = 156,
    MotionPredictionHints               = 157,
    TriggerAnimation                    = 158,
    CameraShake                         = 159,
    PlayerFogSetting                    = 160,
    CorrectPlayerMovePrediction         = 161,
    ItemRegistry                        = 162,
    FilterTextPacket_Deprecated         = 163,
    ClientBoundDebugRenderer            = 164,
    SyncActorProperty                   = 165,
    AddVolumeEntity                     = 166,
    RemoveVolumeEntity                  = 167,
    SimulationType                      = 168,
    NpcDialogue                         = 169,
    EduUriResource                      = 170,
    CreatePhoto                         = 171,
    UpdateSubChunkBlocks                = 172,
    // 173 reserved
    SubChunk                            = 174,
    SubChunkRequest                     = 175,
    PlayerStartItemCooldown             = 176,
    ScriptMessage                       = 177,
    CodeBuilderSource                   = 178,
    TickingAreasLoadStatus              = 179,
    DimensionData                       = 180,
    AgentAction                         = 181,
    ChangeMobProperty                   = 182,
    LessonProgress                      = 183,
    RequestAbility                      = 184,
    RequestPermissions                  = 185,
    ToastRequest                        = 186,
    UpdateAbilities                     = 187,
    UpdateAdventureSettings             = 188,
    DeathInfo                           = 189,
    EditorNetwork                       = 190,
    FeatureRegistry                     = 191,
    ServerStats                         = 192,
    RequestNetworkSettings              = 193,
    GameTestRequest                     = 194,
    GameTestResults                     = 195,
    PlayerClientInputPermissions        = 196,
    // 197 reserved
    CameraPresets                       = 198,
    UnlockedRecipes                     = 199,
    // 200-299 TitleSpecific range
    CameraInstruction                   = 300,
    CompressedBiomeDefinitionList       = 301,
    TrimData                            = 302,
    OpenSign                            = 303,
    AgentAnimation                      = 304,
    RefreshEntitlements                  = 305,
    PlayerToggleCrafterSlotRequest      = 306,
    SetPlayerInventoryOptions           = 307,
    SetHud                              = 308,
    AwardAchievement                    = 309,
    ClientboundCloseForm                = 310,
    // 311 reserved
    ServerboundLoadingScreen            = 312,
    JigsawStructureData                 = 313,
    CurrentStructureFeature             = 314,
    ServerboundDiagnostics              = 315,
    CameraAimAssist                     = 316,
    ContainerRegistryCleanup            = 317,
    MovementEffect                      = 318,
    // 319 reserved
    CameraAimAssistPresets              = 320,
    ClientCameraAimAssist               = 321,
    ClientMovementPredictionSync        = 322,
    UpdateClientOptions                 = 323,
    PlayerVideoCapture                  = 324,
    PlayerUpdateEntityOverrides         = 325,
    PlayerLocation                      = 326,
    ClientboundControlSchemeSet         = 327,
    ServerScriptDebugDrawer             = 328,
    ServerboundPackSettingChange        = 329,
    ClientboundDataStore                = 330,
    GraphicsParameterOverride           = 331,
    ServerboundDataStore                = 332,
    ClientboundDataDrivenUIShowScreen   = 333,
    ClientboundDataDrivenUICloseScreen   = 334,
    ClientboundDataDrivenUIReload       = 335,
    ClientboundTextureShift             = 336,
    VoxelShapes                         = 337,
    CameraSpline                        = 338,
    CameraAimAssistActorPriority        = 339,
    ResourcePacksReadyForValidation     = 340,
    LocatorBar                          = 341,
    PartyChanged                        = 342,
    ServerboundDataDrivenScreenClosed   = 343,
    SyncWorldClocks                     = 344,
    ClientboundAttributeLayerSync       = 345,
    EndId                               = 346
};

/// Returns string name for a packet ID (for debugging/logging)
inline const char* packetIdToString(PacketId id) {
    switch (id) {
        case PacketId::KeepAlive: return "KeepAlive";
        case PacketId::Login: return "Login";
        case PacketId::PlayStatus: return "PlayStatus";
        case PacketId::ServerToClientHandshake: return "ServerToClientHandshake";
        case PacketId::ClientToServerHandshake: return "ClientToServerHandshake";
        case PacketId::Disconnect: return "Disconnect";
        case PacketId::ResourcePacksInfo: return "ResourcePacksInfo";
        case PacketId::ResourcePackStack: return "ResourcePackStack";
        case PacketId::ResourcePackClientResponse: return "ResourcePackClientResponse";
        case PacketId::Text: return "Text";
        case PacketId::SetTime: return "SetTime";
        case PacketId::StartGame: return "StartGame";
        case PacketId::AddPlayer: return "AddPlayer";
        case PacketId::AddActor: return "AddActor";
        case PacketId::RemoveActor: return "RemoveActor";
        case PacketId::AddItemActor: return "AddItemActor";
        case PacketId::ServerPlayerPostMovePosition: return "ServerPlayerPostMovePosition";
        case PacketId::TakeItemActor: return "TakeItemActor";
        case PacketId::MoveAbsoluteActor: return "MoveAbsoluteActor";
        case PacketId::MovePlayer: return "MovePlayer";
        case PacketId::UpdateBlock: return "UpdateBlock";
        case PacketId::AddPainting: return "AddPainting";
        case PacketId::LevelEvent: return "LevelEvent";
        case PacketId::BlockEvent: return "BlockEvent";
        case PacketId::ActorEvent: return "ActorEvent";
        case PacketId::MobEffect: return "MobEffect";
        case PacketId::UpdateAttributes: return "UpdateAttributes";
        case PacketId::InventoryTransaction: return "InventoryTransaction";
        case PacketId::PlayerEquipment: return "PlayerEquipment";
        case PacketId::MobArmorEquipment: return "MobArmorEquipment";
        case PacketId::Interact: return "Interact";
        case PacketId::BlockPickRequest: return "BlockPickRequest";
        case PacketId::ActorPickRequest: return "ActorPickRequest";
        case PacketId::PlayerAction: return "PlayerAction";
        case PacketId::HurtArmor: return "HurtArmor";
        case PacketId::SetActorData: return "SetActorData";
        case PacketId::SetActorMotion: return "SetActorMotion";
        case PacketId::SetActorLink: return "SetActorLink";
        case PacketId::SetHealth: return "SetHealth";
        case PacketId::SetSpawnPosition: return "SetSpawnPosition";
        case PacketId::Animate: return "Animate";
        case PacketId::Respawn: return "Respawn";
        case PacketId::ContainerOpen: return "ContainerOpen";
        case PacketId::ContainerClose: return "ContainerClose";
        case PacketId::PlayerHotbar: return "PlayerHotbar";
        case PacketId::InventoryContent: return "InventoryContent";
        case PacketId::InventorySlot: return "InventorySlot";
        case PacketId::ContainerSetData: return "ContainerSetData";
        case PacketId::CraftingData: return "CraftingData";
        case PacketId::GuiDataPickItem: return "GuiDataPickItem";
        case PacketId::BlockActorData: return "BlockActorData";
        case PacketId::FullChunkData: return "FullChunkData";
        case PacketId::SetCommandsEnabled: return "SetCommandsEnabled";
        case PacketId::SetDifficulty: return "SetDifficulty";
        case PacketId::ChangeDimension: return "ChangeDimension";
        case PacketId::SetPlayerGameType: return "SetPlayerGameType";
        case PacketId::PlayerList: return "PlayerList";
        case PacketId::SimpleEvent: return "SimpleEvent";
        case PacketId::LegacyTelemetryEvent: return "LegacyTelemetryEvent";
        case PacketId::SpawnExperienceOrb: return "SpawnExperienceOrb";
        case PacketId::MapData: return "MapData";
        case PacketId::MapInfoRequest: return "MapInfoRequest";
        case PacketId::RequestChunkRadius: return "RequestChunkRadius";
        case PacketId::ChunkRadiusUpdated: return "ChunkRadiusUpdated";
        case PacketId::GameRulesChanged: return "GameRulesChanged";
        case PacketId::Camera: return "Camera";
        case PacketId::BossEvent: return "BossEvent";
        case PacketId::ShowCredits: return "ShowCredits";
        case PacketId::AvailableCommands: return "AvailableCommands";
        case PacketId::CommandRequest: return "CommandRequest";
        case PacketId::CommandBlockUpdate: return "CommandBlockUpdate";
        case PacketId::CommandOutput: return "CommandOutput";
        case PacketId::UpdateTrade: return "UpdateTrade";
        case PacketId::UpdateEquip: return "UpdateEquip";
        case PacketId::ResourcePackDataInfo: return "ResourcePackDataInfo";
        case PacketId::ResourcePackChunkData: return "ResourcePackChunkData";
        case PacketId::ResourcePackChunkRequest: return "ResourcePackChunkRequest";
        case PacketId::Transfer: return "Transfer";
        case PacketId::PlaySound: return "PlaySound";
        case PacketId::StopSound: return "StopSound";
        case PacketId::SetTitle: return "SetTitle";
        case PacketId::AddBehaviorTree: return "AddBehaviorTree";
        case PacketId::StructureBlockUpdate: return "StructureBlockUpdate";
        case PacketId::ShowStoreOffer: return "ShowStoreOffer";
        case PacketId::PurchaseReceipt: return "PurchaseReceipt";
        case PacketId::PlayerSkin: return "PlayerSkin";
        case PacketId::SubClientLogin: return "SubClientLogin";
        case PacketId::AutomationClientConnect: return "AutomationClientConnect";
        case PacketId::SetLastHurtBy: return "SetLastHurtBy";
        case PacketId::BookEdit: return "BookEdit";
        case PacketId::NpcRequest: return "NpcRequest";
        case PacketId::PhotoTransfer: return "PhotoTransfer";
        case PacketId::ModalFormRequest: return "ModalFormRequest";
        case PacketId::ModalFormResponse: return "ModalFormResponse";
        case PacketId::ServerSettingsRequest: return "ServerSettingsRequest";
        case PacketId::ServerSettingsResponse: return "ServerSettingsResponse";
        case PacketId::ShowProfile: return "ShowProfile";
        case PacketId::SetDefaultGameType: return "SetDefaultGameType";
        case PacketId::RemoveObjective: return "RemoveObjective";
        case PacketId::SetDisplayObjective: return "SetDisplayObjective";
        case PacketId::SetScore: return "SetScore";
        case PacketId::LabTable: return "LabTable";
        case PacketId::UpdateBlockSynced: return "UpdateBlockSynced";
        case PacketId::MoveDeltaActor: return "MoveDeltaActor";
        case PacketId::SetScoreboardIdentity: return "SetScoreboardIdentity";
        case PacketId::SetLocalPlayerAsInit: return "SetLocalPlayerAsInit";
        case PacketId::UpdateSoftEnum: return "UpdateSoftEnum";
        case PacketId::Ping: return "Ping";
        case PacketId::SpawnParticleEffect: return "SpawnParticleEffect";
        case PacketId::AvailableActorIdentifiers: return "AvailableActorIdentifiers";
        case PacketId::NetworkChunkPublisherUpdate: return "NetworkChunkPublisherUpdate";
        case PacketId::BiomeDefinitionList: return "BiomeDefinitionList";
        case PacketId::LevelSoundEvent: return "LevelSoundEvent";
        case PacketId::LevelEventGeneric: return "LevelEventGeneric";
        case PacketId::LecternUpdate: return "LecternUpdate";
        case PacketId::ClientCacheStatus: return "ClientCacheStatus";
        case PacketId::OnScreenTextureAnimation: return "OnScreenTextureAnimation";
        case PacketId::MapCreateLockedCopy: return "MapCreateLockedCopy";
        case PacketId::StructureTemplateDataExportRequest: return "StructureTemplateDataExportRequest";
        case PacketId::StructureTemplateDataExportResponse: return "StructureTemplateDataExportResponse";
        case PacketId::ClientCacheBlobStatus: return "ClientCacheBlobStatus";
        case PacketId::ClientCacheMissResponse: return "ClientCacheMissResponse";
        case PacketId::EducationSettings: return "EducationSettings";
        case PacketId::Emote: return "Emote";
        case PacketId::MultiplayerSettings: return "MultiplayerSettings";
        case PacketId::SettingsCommand: return "SettingsCommand";
        case PacketId::AnvilDamage: return "AnvilDamage";
        case PacketId::CompletedUsingItem: return "CompletedUsingItem";
        case PacketId::NetworkSettings: return "NetworkSettings";
        case PacketId::PlayerAuthInput: return "PlayerAuthInput";
        case PacketId::CreativeContent: return "CreativeContent";
        case PacketId::PlayerEnchantOptions: return "PlayerEnchantOptions";
        case PacketId::ItemStackRequest: return "ItemStackRequest";
        case PacketId::ItemStackResponse: return "ItemStackResponse";
        case PacketId::PlayerArmorDamage: return "PlayerArmorDamage";
        case PacketId::CodeBuilder: return "CodeBuilder";
        case PacketId::UpdatePlayerGameType: return "UpdatePlayerGameType";
        case PacketId::EmoteList: return "EmoteList";
        case PacketId::PositionTrackingDBServerBroadcast: return "PositionTrackingDBServerBroadcast";
        case PacketId::PositionTrackingDBClientRequest: return "PositionTrackingDBClientRequest";
        case PacketId::DebugInfo: return "DebugInfo";
        case PacketId::PacketViolationWarning: return "PacketViolationWarning";
        case PacketId::MotionPredictionHints: return "MotionPredictionHints";
        case PacketId::TriggerAnimation: return "TriggerAnimation";
        case PacketId::CameraShake: return "CameraShake";
        case PacketId::PlayerFogSetting: return "PlayerFogSetting";
        case PacketId::CorrectPlayerMovePrediction: return "CorrectPlayerMovePrediction";
        case PacketId::ItemRegistry: return "ItemRegistry";
        case PacketId::ClientBoundDebugRenderer: return "ClientBoundDebugRenderer";
        case PacketId::SyncActorProperty: return "SyncActorProperty";
        case PacketId::AddVolumeEntity: return "AddVolumeEntity";
        case PacketId::RemoveVolumeEntity: return "RemoveVolumeEntity";
        case PacketId::SimulationType: return "SimulationType";
        case PacketId::NpcDialogue: return "NpcDialogue";
        case PacketId::EduUriResource: return "EduUriResource";
        case PacketId::CreatePhoto: return "CreatePhoto";
        case PacketId::UpdateSubChunkBlocks: return "UpdateSubChunkBlocks";
        case PacketId::SubChunk: return "SubChunk";
        case PacketId::SubChunkRequest: return "SubChunkRequest";
        case PacketId::PlayerStartItemCooldown: return "PlayerStartItemCooldown";
        case PacketId::ScriptMessage: return "ScriptMessage";
        case PacketId::CodeBuilderSource: return "CodeBuilderSource";
        case PacketId::TickingAreasLoadStatus: return "TickingAreasLoadStatus";
        case PacketId::DimensionData: return "DimensionData";
        case PacketId::AgentAction: return "AgentAction";
        case PacketId::ChangeMobProperty: return "ChangeMobProperty";
        case PacketId::LessonProgress: return "LessonProgress";
        case PacketId::RequestAbility: return "RequestAbility";
        case PacketId::RequestPermissions: return "RequestPermissions";
        case PacketId::ToastRequest: return "ToastRequest";
        case PacketId::UpdateAbilities: return "UpdateAbilities";
        case PacketId::UpdateAdventureSettings: return "UpdateAdventureSettings";
        case PacketId::DeathInfo: return "DeathInfo";
        case PacketId::EditorNetwork: return "EditorNetwork";
        case PacketId::FeatureRegistry: return "FeatureRegistry";
        case PacketId::ServerStats: return "ServerStats";
        case PacketId::RequestNetworkSettings: return "RequestNetworkSettings";
        case PacketId::GameTestRequest: return "GameTestRequest";
        case PacketId::GameTestResults: return "GameTestResults";
        case PacketId::PlayerClientInputPermissions: return "PlayerClientInputPermissions";
        case PacketId::CameraPresets: return "CameraPresets";
        case PacketId::UnlockedRecipes: return "UnlockedRecipes";
        case PacketId::CameraInstruction: return "CameraInstruction";
        case PacketId::CompressedBiomeDefinitionList: return "CompressedBiomeDefinitionList";
        case PacketId::TrimData: return "TrimData";
        case PacketId::OpenSign: return "OpenSign";
        case PacketId::AgentAnimation: return "AgentAnimation";
        case PacketId::RefreshEntitlements: return "RefreshEntitlements";
        case PacketId::PlayerToggleCrafterSlotRequest: return "PlayerToggleCrafterSlotRequest";
        case PacketId::SetPlayerInventoryOptions: return "SetPlayerInventoryOptions";
        case PacketId::SetHud: return "SetHud";
        case PacketId::AwardAchievement: return "AwardAchievement";
        case PacketId::ClientboundCloseForm: return "ClientboundCloseForm";
        case PacketId::ServerboundLoadingScreen: return "ServerboundLoadingScreen";
        case PacketId::JigsawStructureData: return "JigsawStructureData";
        case PacketId::CurrentStructureFeature: return "CurrentStructureFeature";
        case PacketId::ServerboundDiagnostics: return "ServerboundDiagnostics";
        case PacketId::CameraAimAssist: return "CameraAimAssist";
        case PacketId::ContainerRegistryCleanup: return "ContainerRegistryCleanup";
        case PacketId::MovementEffect: return "MovementEffect";
        case PacketId::CameraAimAssistPresets: return "CameraAimAssistPresets";
        case PacketId::ClientCameraAimAssist: return "ClientCameraAimAssist";
        case PacketId::ClientMovementPredictionSync: return "ClientMovementPredictionSync";
        case PacketId::UpdateClientOptions: return "UpdateClientOptions";
        case PacketId::PlayerVideoCapture: return "PlayerVideoCapture";
        case PacketId::PlayerUpdateEntityOverrides: return "PlayerUpdateEntityOverrides";
        case PacketId::PlayerLocation: return "PlayerLocation";
        case PacketId::ClientboundControlSchemeSet: return "ClientboundControlSchemeSet";
        case PacketId::ServerScriptDebugDrawer: return "ServerScriptDebugDrawer";
        case PacketId::ServerboundPackSettingChange: return "ServerboundPackSettingChange";
        case PacketId::ClientboundDataStore: return "ClientboundDataStore";
        case PacketId::GraphicsParameterOverride: return "GraphicsParameterOverride";
        case PacketId::ServerboundDataStore: return "ServerboundDataStore";
        case PacketId::ClientboundDataDrivenUIShowScreen: return "ClientboundDataDrivenUIShowScreen";
        case PacketId::ClientboundDataDrivenUICloseScreen: return "ClientboundDataDrivenUICloseScreen";
        case PacketId::ClientboundDataDrivenUIReload: return "ClientboundDataDrivenUIReload";
        case PacketId::ClientboundTextureShift: return "ClientboundTextureShift";
        case PacketId::VoxelShapes: return "VoxelShapes";
        case PacketId::CameraSpline: return "CameraSpline";
        case PacketId::CameraAimAssistActorPriority: return "CameraAimAssistActorPriority";
        case PacketId::ResourcePacksReadyForValidation: return "ResourcePacksReadyForValidation";
        case PacketId::LocatorBar: return "LocatorBar";
        case PacketId::PartyChanged: return "PartyChanged";
        case PacketId::ServerboundDataDrivenScreenClosed: return "ServerboundDataDrivenScreenClosed";
        case PacketId::SyncWorldClocks: return "SyncWorldClocks";
        case PacketId::ClientboundAttributeLayerSync: return "ClientboundAttributeLayerSync";
        default: return "Unknown";
    }
}

} // namespace cphertunnel
