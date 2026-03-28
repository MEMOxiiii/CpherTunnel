// =============================================================================
// CpherTunnel - Minimal NBT (Named Binary Tag) support
// Used by various packets for compound data structures
// =============================================================================
#pragma once

#include "binary_stream.hpp"
#include <variant>
#include <map>
#include <memory>

namespace cphertunnel {

enum class NbtTagType : uint8_t {
    End       = 0,
    Byte      = 1,
    Short     = 2,
    Int       = 3,
    Long      = 4,
    Float     = 5,
    Double    = 6,
    ByteArray = 7,
    String    = 8,
    List      = 9,
    Compound  = 10,
    IntArray   = 11,
    LongArray  = 12
};

class NbtTag;
using NbtCompound = std::map<std::string, std::shared_ptr<NbtTag>>;
using NbtList = std::vector<std::shared_ptr<NbtTag>>;

class NbtTag {
public:
    NbtTagType type = NbtTagType::End;

    // Value storage
    int8_t byteVal = 0;
    int16_t shortVal = 0;
    int32_t intVal = 0;
    int64_t longVal = 0;
    float floatVal = 0.0f;
    double doubleVal = 0.0;
    std::string stringVal;
    ByteBuffer byteArrayVal;
    std::vector<int32_t> intArrayVal;
    std::vector<int64_t> longArrayVal;
    NbtList listVal;
    NbtTagType listTagType = NbtTagType::End;
    NbtCompound compoundVal;

    NbtTag() = default;
    explicit NbtTag(NbtTagType t) : type(t) {}

    // ─── Factory methods ─────────────────────────────────────────
    static std::shared_ptr<NbtTag> makeByte(int8_t v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Byte);
        t->byteVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeShort(int16_t v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Short);
        t->shortVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeInt(int32_t v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Int);
        t->intVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeLong(int64_t v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Long);
        t->longVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeFloat(float v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Float);
        t->floatVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeDouble(double v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::Double);
        t->doubleVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeString(const std::string& v) {
        auto t = std::make_shared<NbtTag>(NbtTagType::String);
        t->stringVal = v; return t;
    }
    static std::shared_ptr<NbtTag> makeCompound() {
        return std::make_shared<NbtTag>(NbtTagType::Compound);
    }

    // ─── NBT Little-Endian serialization (Bedrock uses LE NBT) ──
    static void writeTag(BinaryStream& stream, const NbtTag& tag) {
        switch (tag.type) {
            case NbtTagType::End: break;
            case NbtTagType::Byte:
                stream.writeSignedByte(tag.byteVal); break;
            case NbtTagType::Short:
                stream.writeSignedShort(tag.shortVal); break;
            case NbtTagType::Int:
                stream.writeSignedInt(tag.intVal); break;
            case NbtTagType::Long:
                stream.writeSignedInt64(tag.longVal); break;
            case NbtTagType::Float:
                stream.writeFloat(tag.floatVal); break;
            case NbtTagType::Double:
                stream.writeDouble(tag.doubleVal); break;
            case NbtTagType::ByteArray:
                stream.writeSignedInt(static_cast<int32_t>(tag.byteArrayVal.size()));
                stream.write(tag.byteArrayVal.data(), tag.byteArrayVal.size());
                break;
            case NbtTagType::String: {
                stream.writeUnsignedShort(static_cast<uint16_t>(tag.stringVal.size()));
                stream.write(tag.stringVal.data(), tag.stringVal.size());
                break;
            }
            case NbtTagType::List:
                stream.writeByte(static_cast<uint8_t>(tag.listTagType));
                stream.writeSignedInt(static_cast<int32_t>(tag.listVal.size()));
                for (auto& item : tag.listVal) {
                    writeTag(stream, *item);
                }
                break;
            case NbtTagType::Compound:
                for (auto& [name, child] : tag.compoundVal) {
                    stream.writeByte(static_cast<uint8_t>(child->type));
                    stream.writeUnsignedShort(static_cast<uint16_t>(name.size()));
                    stream.write(name.data(), name.size());
                    writeTag(stream, *child);
                }
                stream.writeByte(static_cast<uint8_t>(NbtTagType::End));
                break;
            case NbtTagType::IntArray:
                stream.writeSignedInt(static_cast<int32_t>(tag.intArrayVal.size()));
                for (auto v : tag.intArrayVal) stream.writeSignedInt(v);
                break;
            case NbtTagType::LongArray:
                stream.writeSignedInt(static_cast<int32_t>(tag.longArrayVal.size()));
                for (auto v : tag.longArrayVal) stream.writeSignedInt64(v);
                break;
        }
    }

    static std::shared_ptr<NbtTag> readTag(ReadOnlyBinaryStream& stream, NbtTagType type) {
        auto tag = std::make_shared<NbtTag>(type);
        switch (type) {
            case NbtTagType::End: break;
            case NbtTagType::Byte:
                tag->byteVal = stream.readSignedByte(); break;
            case NbtTagType::Short:
                tag->shortVal = stream.readSignedShort(); break;
            case NbtTagType::Int:
                tag->intVal = stream.readSignedInt(); break;
            case NbtTagType::Long:
                tag->longVal = stream.readSignedInt64(); break;
            case NbtTagType::Float:
                tag->floatVal = stream.readFloat(); break;
            case NbtTagType::Double:
                tag->doubleVal = stream.readDouble(); break;
            case NbtTagType::ByteArray: {
                int32_t len = stream.readSignedInt();
                if (len > 0 && static_cast<size_t>(len) < MAX_PACKET_SIZE) {
                    tag->byteArrayVal.resize(len);
                    stream.read(tag->byteArrayVal.data(), len);
                }
                break;
            }
            case NbtTagType::String: {
                uint16_t len = stream.readUnsignedShort();
                if (len > 0 && len <= 32767) {
                    tag->stringVal.resize(len);
                    stream.read(tag->stringVal.data(), len);
                }
                break;
            }
            case NbtTagType::List: {
                tag->listTagType = static_cast<NbtTagType>(stream.readByte());
                int32_t count = stream.readSignedInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->listVal.push_back(readTag(stream, tag->listTagType));
                }
                break;
            }
            case NbtTagType::Compound: {
                while (!stream.hasOverflowed()) {
                    auto childType = static_cast<NbtTagType>(stream.readByte());
                    if (childType == NbtTagType::End) break;
                    uint16_t nameLen = stream.readUnsignedShort();
                    std::string name(nameLen, '\0');
                    stream.read(name.data(), nameLen);
                    tag->compoundVal[name] = readTag(stream, childType);
                }
                break;
            }
            case NbtTagType::IntArray: {
                int32_t count = stream.readSignedInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->intArrayVal.push_back(stream.readSignedInt());
                }
                break;
            }
            case NbtTagType::LongArray: {
                int32_t count = stream.readSignedInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->longArrayVal.push_back(stream.readSignedInt64());
                }
                break;
            }
        }
        return tag;
    }

    /// Write a root compound (with type byte + name)
    static void writeRootCompound(BinaryStream& stream, const NbtTag& tag) {
        stream.writeByte(static_cast<uint8_t>(NbtTagType::Compound));
        stream.writeUnsignedShort(0); // empty root name
        writeTag(stream, tag);
    }

    /// Read a root compound
    static std::shared_ptr<NbtTag> readRootCompound(ReadOnlyBinaryStream& stream) {
        auto type = static_cast<NbtTagType>(stream.readByte());
        if (type != NbtTagType::Compound) return nullptr;
        stream.readUnsignedShort(); // skip root name
        return readTag(stream, NbtTagType::Compound);
    }

    // ─── Network Little-Endian NBT (Bedrock network protocol format) ─
    // Differs from standard LE NBT:
    //   - String lengths use unsigned VarInt instead of unsigned short
    //   - TAG_Int uses zigzag VarInt32 instead of LE int32
    //   - TAG_Long uses zigzag VarInt64 instead of LE int64
    //   - List/Array sizes use zigzag VarInt32 instead of LE int32
    //   - TAG_Byte, TAG_Short, TAG_Float, TAG_Double remain unchanged

    static void writeNetworkTag(BinaryStream& stream, const NbtTag& tag) {
        switch (tag.type) {
            case NbtTagType::End: break;
            case NbtTagType::Byte:
                stream.writeSignedByte(tag.byteVal); break;
            case NbtTagType::Short:
                stream.writeSignedShort(tag.shortVal); break;
            case NbtTagType::Int:
                stream.writeVarInt(tag.intVal); break;
            case NbtTagType::Long:
                stream.writeVarInt64(tag.longVal); break;
            case NbtTagType::Float:
                stream.writeFloat(tag.floatVal); break;
            case NbtTagType::Double:
                stream.writeDouble(tag.doubleVal); break;
            case NbtTagType::ByteArray:
                stream.writeVarInt(static_cast<int32_t>(tag.byteArrayVal.size()));
                stream.write(tag.byteArrayVal.data(), tag.byteArrayVal.size());
                break;
            case NbtTagType::String: {
                stream.writeUnsignedVarInt(static_cast<uint32_t>(tag.stringVal.size()));
                stream.write(tag.stringVal.data(), tag.stringVal.size());
                break;
            }
            case NbtTagType::List:
                stream.writeByte(static_cast<uint8_t>(tag.listTagType));
                stream.writeVarInt(static_cast<int32_t>(tag.listVal.size()));
                for (auto& item : tag.listVal) {
                    writeNetworkTag(stream, *item);
                }
                break;
            case NbtTagType::Compound:
                for (auto& [name, child] : tag.compoundVal) {
                    stream.writeByte(static_cast<uint8_t>(child->type));
                    stream.writeUnsignedVarInt(static_cast<uint32_t>(name.size()));
                    stream.write(name.data(), name.size());
                    writeNetworkTag(stream, *child);
                }
                stream.writeByte(static_cast<uint8_t>(NbtTagType::End));
                break;
            case NbtTagType::IntArray:
                stream.writeVarInt(static_cast<int32_t>(tag.intArrayVal.size()));
                for (auto v : tag.intArrayVal) stream.writeVarInt(v);
                break;
            case NbtTagType::LongArray:
                stream.writeVarInt(static_cast<int32_t>(tag.longArrayVal.size()));
                for (auto v : tag.longArrayVal) stream.writeVarInt64(v);
                break;
        }
    }

    static std::shared_ptr<NbtTag> readNetworkTag(ReadOnlyBinaryStream& stream, NbtTagType type) {
        auto tag = std::make_shared<NbtTag>(type);
        switch (type) {
            case NbtTagType::End: break;
            case NbtTagType::Byte:
                tag->byteVal = stream.readSignedByte(); break;
            case NbtTagType::Short:
                tag->shortVal = stream.readSignedShort(); break;
            case NbtTagType::Int:
                tag->intVal = stream.readVarInt(); break;
            case NbtTagType::Long:
                tag->longVal = stream.readVarInt64(); break;
            case NbtTagType::Float:
                tag->floatVal = stream.readFloat(); break;
            case NbtTagType::Double:
                tag->doubleVal = stream.readDouble(); break;
            case NbtTagType::ByteArray: {
                int32_t len = stream.readVarInt();
                if (len > 0 && static_cast<size_t>(len) < MAX_PACKET_SIZE) {
                    tag->byteArrayVal.resize(len);
                    stream.read(tag->byteArrayVal.data(), len);
                }
                break;
            }
            case NbtTagType::String: {
                uint32_t len = stream.readUnsignedVarInt();
                if (len > 0 && len <= 32767) {
                    tag->stringVal.resize(len);
                    stream.read(tag->stringVal.data(), len);
                }
                break;
            }
            case NbtTagType::List: {
                tag->listTagType = static_cast<NbtTagType>(stream.readByte());
                int32_t count = stream.readVarInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->listVal.push_back(readNetworkTag(stream, tag->listTagType));
                }
                break;
            }
            case NbtTagType::Compound: {
                while (!stream.hasOverflowed()) {
                    auto childType = static_cast<NbtTagType>(stream.readByte());
                    if (childType == NbtTagType::End) break;
                    uint32_t nameLen = stream.readUnsignedVarInt();
                    std::string name(nameLen, '\0');
                    stream.read(name.data(), nameLen);
                    tag->compoundVal[name] = readNetworkTag(stream, childType);
                }
                break;
            }
            case NbtTagType::IntArray: {
                int32_t count = stream.readVarInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->intArrayVal.push_back(stream.readVarInt());
                }
                break;
            }
            case NbtTagType::LongArray: {
                int32_t count = stream.readVarInt();
                for (int32_t i = 0; i < count && !stream.hasOverflowed(); ++i) {
                    tag->longArrayVal.push_back(stream.readVarInt64());
                }
                break;
            }
        }
        return tag;
    }

    /// Write NBT using Network Little-Endian format (for network packets)
    static void writeNetworkNbt(BinaryStream& stream, const NbtTag& tag) {
        stream.writeByte(static_cast<uint8_t>(NbtTagType::Compound));
        stream.writeUnsignedVarInt(0); // empty root name (VarUint length)
        writeNetworkTag(stream, tag);
    }

    /// Read NBT using Network Little-Endian format (for network packets)
    static std::shared_ptr<NbtTag> readNetworkNbt(ReadOnlyBinaryStream& stream) {
        auto type = static_cast<NbtTagType>(stream.readByte());
        if (type != NbtTagType::Compound) return nullptr;
        stream.readUnsignedVarInt(); // skip root name
        return readNetworkTag(stream, NbtTagType::Compound);
    }
};

} // namespace cphertunnel
