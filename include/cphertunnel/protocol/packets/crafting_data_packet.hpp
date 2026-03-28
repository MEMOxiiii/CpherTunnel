// CpherTunnel - CraftingDataPacket (ID: 52)
#pragma once
#include "../packet.hpp"
#include "../packet_types.hpp"
namespace cphertunnel {

// ─── Recipe sub-types ───────────────────────────────────────────
struct RecipeIngredient {
    uint8_t internal_type = 0;
    int32_t stack_size = 0;
    void write(BinaryStream& s) const { s.writeByte(internal_type); s.writeVarInt(stack_size); }
    void read(ReadOnlyBinaryStream& s) { internal_type = s.readByte(); stack_size = s.readVarInt(); }
};

struct NetworkItemInstanceDescriptorSimple {
    int32_t id = 0;
    uint16_t stack_size = 0;
    uint32_t aux_value = 0;
    int32_t block_runtime_id = 0;
    std::string user_data;
    void write(BinaryStream& s) const {
        s.writeVarInt(id);
        if (id == 0) return;
        s.writeUnsignedShort(stack_size);
        s.writeUnsignedVarInt(aux_value);
        s.writeVarInt(block_runtime_id);
        s.writeString(user_data);
    }
    void read(ReadOnlyBinaryStream& s) {
        id = s.readVarInt();
        if (id == 0) return;
        stack_size = s.readUnsignedShort();
        aux_value = s.readUnsignedVarInt();
        block_runtime_id = s.readVarInt();
        user_data = s.readString();
    }
};

struct RecipeUnlockingRequirement {
    uint8_t unlocking_context = 0;
    std::vector<RecipeIngredient> ingredients;
    void write(BinaryStream& s) const {
        s.writeByte(unlocking_context);
        if (unlocking_context != 0) {
            s.writeUnsignedVarInt(static_cast<uint32_t>(ingredients.size()));
            for (auto& i : ingredients) i.write(s);
        }
    }
    void read(ReadOnlyBinaryStream& s) {
        unlocking_context = s.readByte();
        if (unlocking_context != 0) {
            uint32_t c = s.readUnsignedVarInt();
            ingredients.resize(c);
            for (auto& i : ingredients) i.read(s);
        }
    }
};

struct ShapelessRecipe {
    std::string recipe_unique_id;
    std::vector<RecipeIngredient> ingredients;
    std::vector<NetworkItemInstanceDescriptorSimple> results;
    UUID recipe_id;
    std::string recipe_tag;
    int32_t priority = 0;
    RecipeUnlockingRequirement unlocking_req;
    void write(BinaryStream& s) const {
        s.writeString(recipe_unique_id);
        s.writeUnsignedVarInt(static_cast<uint32_t>(ingredients.size()));
        for (auto& i : ingredients) i.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(results.size()));
        for (auto& r : results) r.write(s);
        s.writeUUID(recipe_id);
        s.writeString(recipe_tag);
        s.writeVarInt(priority);
        unlocking_req.write(s);
    }
    void read(ReadOnlyBinaryStream& s) {
        recipe_unique_id = s.readString();
        uint32_t ic = s.readUnsignedVarInt();
        ingredients.resize(ic); for (auto& i : ingredients) i.read(s);
        uint32_t rc = s.readUnsignedVarInt();
        results.resize(rc); for (auto& r : results) r.read(s);
        recipe_id = s.readUUID();
        recipe_tag = s.readString();
        priority = s.readVarInt();
        unlocking_req.read(s);
    }
};

struct ShapedRecipe {
    std::string recipe_unique_id;
    int32_t width = 0, height = 0;
    std::vector<RecipeIngredient> ingredients; // width * height
    std::vector<NetworkItemInstanceDescriptorSimple> results;
    UUID recipe_id;
    std::string recipe_tag;
    int32_t priority = 0;
    bool assume_symmetry = false;
    RecipeUnlockingRequirement unlocking_req;
    void write(BinaryStream& s) const {
        s.writeString(recipe_unique_id);
        s.writeVarInt(width); s.writeVarInt(height);
        for (auto& i : ingredients) i.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(results.size()));
        for (auto& r : results) r.write(s);
        s.writeUUID(recipe_id);
        s.writeString(recipe_tag);
        s.writeVarInt(priority);
        s.writeBool(assume_symmetry);
        unlocking_req.write(s);
    }
    void read(ReadOnlyBinaryStream& s) {
        recipe_unique_id = s.readString();
        width = s.readVarInt(); height = s.readVarInt();
        ingredients.resize(static_cast<size_t>(width * height));
        for (auto& i : ingredients) i.read(s);
        uint32_t rc = s.readUnsignedVarInt();
        results.resize(rc); for (auto& r : results) r.read(s);
        recipe_id = s.readUUID();
        recipe_tag = s.readString();
        priority = s.readVarInt();
        assume_symmetry = s.readBool();
        unlocking_req.read(s);
    }
};

struct FurnaceRecipe {
    int32_t item_data = 0;
    NetworkItemInstanceDescriptorSimple result;
    std::string recipe_tag;
    void write(BinaryStream& s) const { s.writeVarInt(item_data); result.write(s); s.writeString(recipe_tag); }
    void read(ReadOnlyBinaryStream& s) { item_data = s.readVarInt(); result.read(s); recipe_tag = s.readString(); }
};

struct FurnaceAuxRecipe {
    int32_t item_data = 0;
    int32_t aux_data = 0;
    NetworkItemInstanceDescriptorSimple result;
    std::string recipe_tag;
    void write(BinaryStream& s) const { s.writeVarInt(item_data); s.writeVarInt(aux_data); result.write(s); s.writeString(recipe_tag); }
    void read(ReadOnlyBinaryStream& s) { item_data = s.readVarInt(); aux_data = s.readVarInt(); result.read(s); recipe_tag = s.readString(); }
};

struct MultiRecipe {
    UUID uuid;
    uint32_t net_id = 0;
    void write(BinaryStream& s) const { s.writeUUID(uuid); s.writeUnsignedVarInt(net_id); }
    void read(ReadOnlyBinaryStream& s) { uuid = s.readUUID(); net_id = s.readUnsignedVarInt(); }
};

struct SmithingTransformRecipe {
    std::string recipe_id;
    RecipeIngredient template_ingredient;
    RecipeIngredient base_ingredient;
    RecipeIngredient addition_ingredient;
    NetworkItemInstanceDescriptorSimple result;
    std::string tag;
    void write(BinaryStream& s) const {
        s.writeString(recipe_id); template_ingredient.write(s); base_ingredient.write(s);
        addition_ingredient.write(s); result.write(s); s.writeString(tag);
    }
    void read(ReadOnlyBinaryStream& s) {
        recipe_id = s.readString(); template_ingredient.read(s); base_ingredient.read(s);
        addition_ingredient.read(s); result.read(s); tag = s.readString();
    }
};

struct SmithingTrimRecipe {
    std::string recipe_id;
    RecipeIngredient template_ingredient;
    RecipeIngredient base_ingredient;
    RecipeIngredient addition_ingredient;
    std::string tag;
    void write(BinaryStream& s) const {
        s.writeString(recipe_id); template_ingredient.write(s);
        base_ingredient.write(s); addition_ingredient.write(s); s.writeString(tag);
    }
    void read(ReadOnlyBinaryStream& s) {
        recipe_id = s.readString(); template_ingredient.read(s);
        base_ingredient.read(s); addition_ingredient.read(s); tag = s.readString();
    }
};

struct PotionMixDataEntry {
    int32_t from_potion_id = 0, from_item_aux = 0;
    int32_t reagent_item_id = 0, reagent_item_aux = 0;
    int32_t to_potion_id = 0, to_item_aux = 0;
    void write(BinaryStream& s) const {
        s.writeVarInt(from_potion_id); s.writeVarInt(from_item_aux);
        s.writeVarInt(reagent_item_id); s.writeVarInt(reagent_item_aux);
        s.writeVarInt(to_potion_id); s.writeVarInt(to_item_aux);
    }
    void read(ReadOnlyBinaryStream& s) {
        from_potion_id = s.readVarInt(); from_item_aux = s.readVarInt();
        reagent_item_id = s.readVarInt(); reagent_item_aux = s.readVarInt();
        to_potion_id = s.readVarInt(); to_item_aux = s.readVarInt();
    }
};

struct ContainerMixDataEntry {
    int32_t from_item_id = 0, reagent_item_id = 0, to_item_id = 0;
    void write(BinaryStream& s) const { s.writeVarInt(from_item_id); s.writeVarInt(reagent_item_id); s.writeVarInt(to_item_id); }
    void read(ReadOnlyBinaryStream& s) { from_item_id = s.readVarInt(); reagent_item_id = s.readVarInt(); to_item_id = s.readVarInt(); }
};

struct MaterialReducerDataEntry {
    int32_t from_item = 0;
    struct OutputItem { int32_t item_id = 0; int32_t count = 0; };
    std::vector<OutputItem> outputs;
    void write(BinaryStream& s) const {
        s.writeVarInt(from_item);
        s.writeUnsignedVarInt(static_cast<uint32_t>(outputs.size()));
        for (auto& o : outputs) { s.writeVarInt(o.item_id); s.writeVarInt(o.count); }
    }
    void read(ReadOnlyBinaryStream& s) {
        from_item = s.readVarInt();
        uint32_t c = s.readUnsignedVarInt();
        outputs.resize(c);
        for (auto& o : outputs) { o.item_id = s.readVarInt(); o.count = s.readVarInt(); }
    }
};

// ─── CraftingDataEntry ──────────────────────────────────────────
struct CraftingDataEntry {
    enum class Type : int32_t {
        Shapeless = 0, Shaped = 1, Furnace = 2, FurnaceAux = 3,
        Multi = 4, UserDataShapeless = 5, ShapelessChemistry = 6,
        ShapedChemistry = 7, SmithingTransform = 8, SmithingTrim = 9
    };
    Type type = Type::Shapeless;
    ShapelessRecipe shapeless;
    ShapedRecipe shaped;
    FurnaceRecipe furnace;
    FurnaceAuxRecipe furnace_aux;
    MultiRecipe multi;
    SmithingTransformRecipe smithing_transform;
    SmithingTrimRecipe smithing_trim;
    uint32_t net_id = 0; // for types 0,1,5,6,7,8,9

    void write(BinaryStream& s) const {
        s.writeVarInt(static_cast<int32_t>(type));
        switch (type) {
            case Type::Shapeless:
            case Type::UserDataShapeless:
            case Type::ShapelessChemistry:
                shapeless.write(s); s.writeUnsignedVarInt(net_id); break;
            case Type::Shaped:
            case Type::ShapedChemistry:
                shaped.write(s); s.writeUnsignedVarInt(net_id); break;
            case Type::Furnace: furnace.write(s); break;
            case Type::FurnaceAux: furnace_aux.write(s); break;
            case Type::Multi: multi.write(s); break;
            case Type::SmithingTransform:
                smithing_transform.write(s); s.writeUnsignedVarInt(net_id); break;
            case Type::SmithingTrim:
                smithing_trim.write(s); s.writeUnsignedVarInt(net_id); break;
        }
    }
    void read(ReadOnlyBinaryStream& s) {
        type = static_cast<Type>(s.readVarInt());
        switch (type) {
            case Type::Shapeless:
            case Type::UserDataShapeless:
            case Type::ShapelessChemistry:
                shapeless.read(s); net_id = s.readUnsignedVarInt(); break;
            case Type::Shaped:
            case Type::ShapedChemistry:
                shaped.read(s); net_id = s.readUnsignedVarInt(); break;
            case Type::Furnace: furnace.read(s); break;
            case Type::FurnaceAux: furnace_aux.read(s); break;
            case Type::Multi: multi.read(s); break;
            case Type::SmithingTransform:
                smithing_transform.read(s); net_id = s.readUnsignedVarInt(); break;
            case Type::SmithingTrim:
                smithing_trim.read(s); net_id = s.readUnsignedVarInt(); break;
        }
    }
};

// ─── CraftingDataPacket ─────────────────────────────────────────
class CraftingDataPacket : public Packet {
public:
    CPHERTUNNEL_PACKET(CraftingDataPacket, CraftingData)

    std::vector<CraftingDataEntry> entries;
    std::vector<PotionMixDataEntry> potion_mixes;
    std::vector<ContainerMixDataEntry> container_mixes;
    std::vector<MaterialReducerDataEntry> material_reducers;
    bool clear_recipes = false;

    void write(BinaryStream& s) const override {
        s.writeUnsignedVarInt(static_cast<uint32_t>(entries.size()));
        for (auto& e : entries) e.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(potion_mixes.size()));
        for (auto& p : potion_mixes) p.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(container_mixes.size()));
        for (auto& c : container_mixes) c.write(s);
        s.writeUnsignedVarInt(static_cast<uint32_t>(material_reducers.size()));
        for (auto& m : material_reducers) m.write(s);
        s.writeBool(clear_recipes);
    }
    void read(ReadOnlyBinaryStream& s) override {
        uint32_t ec = s.readUnsignedVarInt();
        entries.resize(ec); for (auto& e : entries) e.read(s);
        uint32_t pc = s.readUnsignedVarInt();
        potion_mixes.resize(pc); for (auto& p : potion_mixes) p.read(s);
        uint32_t cc = s.readUnsignedVarInt();
        container_mixes.resize(cc); for (auto& c : container_mixes) c.read(s);
        uint32_t mc = s.readUnsignedVarInt();
        material_reducers.resize(mc); for (auto& m : material_reducers) m.read(s);
        clear_recipes = s.readBool();
    }
};

} // namespace cphertunnel
