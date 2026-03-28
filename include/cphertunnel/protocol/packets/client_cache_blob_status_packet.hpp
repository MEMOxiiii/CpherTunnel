// CpherTunnel - ClientCacheBlobStatusPacket (ID: 135)
#pragma once
#include "../packet.hpp"
namespace cphertunnel {
class ClientCacheBlobStatusPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientCacheBlobStatusPacket, ClientCacheBlobStatus)
    std::vector<uint64_t> missing_ids;
    std::vector<uint64_t> found_ids;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(missing_ids.size()));
        s.writeUnsignedVarInt(static_cast<uint32_t>(found_ids.size()));
        for (auto id : missing_ids) s.writeUnsignedInt64(id);
        for (auto id : found_ids) s.writeUnsignedInt64(id);
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t missing_count = s.readUnsignedVarInt();
        uint32_t found_count = s.readUnsignedVarInt();
        missing_ids.resize(missing_count);
        for (auto& id : missing_ids) id = s.readUnsignedInt64();
        found_ids.resize(found_count);
        for (auto& id : found_ids) id = s.readUnsignedInt64();
    }
};
} // namespace cphertunnel
