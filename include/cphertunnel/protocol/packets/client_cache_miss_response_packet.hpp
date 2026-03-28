// CpherTunnel - ClientCacheMissResponsePacket (ID: 136)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class ClientCacheMissResponsePacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientCacheMissResponsePacket, ClientCacheMissResponse)
    struct BlobEntry { uint64_t blob_id = 0; std::string blob_data; };
    std::vector<BlobEntry> missing_blobs;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(missing_blobs.size()));
        for (auto& b : missing_blobs) {
            s.writeUnsignedInt64(b.blob_id);
            s.writeString(b.blob_data);
        }
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t count = s.readUnsignedVarInt();
        missing_blobs.resize(count);
        for (auto& b : missing_blobs) {
            b.blob_id = s.readUnsignedInt64();
            b.blob_data = s.readString();
        }
    }
};
} // namespace cphertunnel
