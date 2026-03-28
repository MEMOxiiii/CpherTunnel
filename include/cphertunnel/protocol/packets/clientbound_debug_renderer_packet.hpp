// CpherTunnel - ClientboundDebugRendererPacket
#pragma once
#include "../packet.hpp"
namespace cphertunnel {

class ClientboundDebugRendererPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(ClientboundDebugRendererPacket, ClientBoundDebugRenderer)

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

} // namespace cphertunnel
