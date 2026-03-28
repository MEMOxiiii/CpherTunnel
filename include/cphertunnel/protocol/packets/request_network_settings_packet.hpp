// CpherTunnel - RequestNetworkSettingsPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {


enum class PlayStatusType : int32_t {
    LoginSuccess                          = 0,
    LoginFailed_ClientOld                 = 1,
    LoginFailed_ServerOld                 = 2,
    PlayerSpawn                           = 3,
    LoginFailed_InvalidTenant             = 4,
    LoginFailed_EditionMismatchEduToVanilla = 5,
    LoginFailed_EditionMismatchVanillaToEdu = 6,
    LoginFailed_ServerFullSubClient       = 7,
    LoginFailed_EditorMismatchEditorToVanilla = 8,
    LoginFailed_EditorMismatchVanillaToEditor = 9,
};
enum class ResourcePackResponseType : uint8_t {
    None       = 0,
    Refused    = 1,
    SendPacks  = 2,
    AllPacksDownloaded = 3,
    Completed  = 4
};
enum class DisconnectFailReason : int32_t {
    Unknown = 0,
    CantConnectNoInternet = 1,
    NoPermissions = 2,
    UnrecoverableError = 3,
    ThirdPartyBlocked = 4,
    ThirdPartyNoInternet = 5,
    ThirdPartyBadLogin = 6,
    FullServer = 7,
    NoConnection = 8,
    PlayerNotFound = 9,
    PacketIdOutOfRange = 10,
    BadPacketHeader = 11,
    BadSubClientLogin = 12,
    OutOfOrderPacket = 13,
    ResourcePackException = 14,
    IncompatibleProtocol = 15,
    CdnHashError = 16,
    IncompatibleTenantedPlayer = 17,
    NotAuthenticated = 18,
    PlayerLeave = 19,
    ConnectionTimeout = 20,
    ServerShutdown = 21,
    PlayerDuplicate = 22,
    OtherError = 23,
};

class RequestNetworkSettingsPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(RequestNetworkSettingsPacket, RequestNetworkSettings)

    int32_t client_network_version = PROTOCOL_VERSION;

    void write(BinaryStream& stream) const override {
        stream.writeSignedBigEndianInt(client_network_version);
    }

    void read(ReadOnlyBinaryStream& stream) override {
        client_network_version = stream.readSignedBigEndianInt();
    }

    Compressibility getCompressibility() const override {
        return Compressibility::Incompressible;
    }

    bool disallowBatching() const override { return true; }
};

} // namespace cphertunnel
