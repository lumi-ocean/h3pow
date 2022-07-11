#include "pch.h"
#include "gamehooks.h"

#include "patcher.h"
#include "asset.h"

#include <h3api/h3api.hpp>

constexpr int OBJECT_TYPES = 232;
constexpr int OBJECT_TYPE_MONSTER = 54;
constexpr int OBJECT_TYPE_TOWN = 98;

Game* game = nullptr;

F_RESOLVER(Town) { return game->towns; }
F_RESOLVER(Terrain) { return game->terrains; }
F_RESOLVER(Resource) { return game->resources; }
F_RESOLVER(Creature) { return game->creatures; }

h3::H3CreatureInformation active_creature_info[MAX_CREATURES] = {};
h3::H3CreatureAnimation active_creature_anim[MAX_CREATURES] = {};

Creature fallback_creature;

enum class MapVersion
{
	roe = 0x0E,
	blade = 0x15,
	sod = 0x1C,
	hota = 0x20,
	wog = 0x33,
	pow = 0x40,
};

int GetMapVersionDefCadre(int version)
{
	switch (MapVersion(version))
	{
	case MapVersion::roe:
		return 0;
	case MapVersion::blade:
		return 1;
	case MapVersion::sod:
		return 2;
	case MapVersion::hota:
		return 3;
	case MapVersion::wog:
		return 4;
	case MapVersion::pow:
		return 5;
	default:
		return 6; // empty icon
	}
}

int Random(int size)
{
	return h3::H3Random::Rand(0, size - 1);
}

void UpdateCreatures()
{
	fallback_creature.set_parent(&game->creatures);
	fallback_creature.native_slot = MAX_CREATURES - 1;

	for (int slot = 0; slot < MAX_CREATURES; slot++)
	{
		Creature* creature = game->creatures.by_native_slot[slot];
		if (creature == nullptr) creature = &fallback_creature;

		Town* native_town = creature->native_town;

		active_creature_info[slot].town = native_town != nullptr ? int(native_town->native_slot) : -1;
		active_creature_info[slot].level = creature->level;
		active_creature_info[slot].soundName = const_char(creature->sound_prefix);
		active_creature_info[slot].defName = const_char(creature->battle_def);

		active_creature_info[slot].doubleWide = creature->double_wide;
		active_creature_info[slot].flyer = creature->flying;
		active_creature_info[slot].shooter = creature->shooting;
		active_creature_info[slot].extendedAttack = creature->dragon_breath;
		active_creature_info[slot].alive = creature->is_alive;
		active_creature_info[slot].destroyWalls = creature->can_attack_walls;
		active_creature_info[slot].siegeWeapon = creature->is_siege_weapon;

		active_creature_info[slot].king1 = creature->king_level == 1;
		active_creature_info[slot].king2 = creature->king_level == 2;
		active_creature_info[slot].king3 = creature->king_level == 3;

		active_creature_info[slot].mindImmunity = creature->mind_immunity;
		active_creature_info[slot].shootsRay = creature->shooting_ray;
		active_creature_info[slot].noMeleePenalty = creature->no_melee_penalty;
		active_creature_info[slot].unk2000 = 0;
		active_creature_info[slot].fireImmunity = creature->fire_immunity;
		active_creature_info[slot].doubleAttack = creature->attack_twice;
		active_creature_info[slot].noRetaliation = creature->no_retaliation;
		active_creature_info[slot].noMorale = creature->no_morale;
		active_creature_info[slot].undead = creature->is_undead;
		active_creature_info[slot].attackAllAround = creature->attack_all_around;
		active_creature_info[slot].fireballAttack = creature->fireball_attack;
		active_creature_info[slot].cannotMove = 0;
		active_creature_info[slot].summon = 0;
		active_creature_info[slot].clone = 0;
		active_creature_info[slot].morale = 0;
		active_creature_info[slot].waiting = 0;
		active_creature_info[slot].done = 0;
		active_creature_info[slot].defending = 0;
		active_creature_info[slot].sacrificed = 0;
		active_creature_info[slot].noColoring = 0;
		active_creature_info[slot].gray = 0;
		active_creature_info[slot].dragon = creature->is_dragon;

		active_creature_info[slot].nameSingular = const_char(creature->name_singular);
		active_creature_info[slot].namePlural = const_char(creature->name_plural);
		active_creature_info[slot].description = const_char(creature->description);

		active_creature_info[slot].cost.wood = creature->cost["wood"];
		active_creature_info[slot].cost.mercury = creature->cost["mercury"];
		active_creature_info[slot].cost.ore = creature->cost["ore"];
		active_creature_info[slot].cost.sulfur = creature->cost["sulfur"];
		active_creature_info[slot].cost.crystal = creature->cost["crystal"];
		active_creature_info[slot].cost.gems = creature->cost["gems"];
		active_creature_info[slot].cost.gold = creature->cost["gold"];

		active_creature_info[slot].fightValue = creature->fight_value;
		active_creature_info[slot].aiValue = creature->ai_value;
		active_creature_info[slot].grow = creature->growth;
		active_creature_info[slot].hGrow = creature->horde_growth;
		active_creature_info[slot].hitPoints = creature->hit_points;
		active_creature_info[slot].speed = creature->speed;
		active_creature_info[slot].attack = creature->attack;
		active_creature_info[slot].defence = creature->defence;
		active_creature_info[slot].damageLow = creature->damage_low;
		active_creature_info[slot].damageHigh = creature->damage_high;
		active_creature_info[slot].numberShots = creature->num_shots;
		active_creature_info[slot].spellCharges = creature->num_spells;
		active_creature_info[slot].advMapLow = creature->adv_map_low;
		active_creature_info[slot].advMapHigh = creature->adv_map_high;

		for (int i = 0; i < 3; i++)
		{
			active_creature_anim[slot].missiles[i].offset_x = creature->animation.missile_offsets[i].x;
			active_creature_anim[slot].missiles[i].offset_y = creature->animation.missile_offsets[i].y;
		}

		for (int i = 0; i < 12; i++)
		{
			active_creature_anim[slot].missileFrameAngles[i] = creature->animation.missile_frame_angles[i];
		}

		active_creature_anim[slot].troopCountLocationOffset = creature->animation.troop_count_location_offset;
		active_creature_anim[slot].attackClimaxFrame = creature->animation.attack_climax_frame;
		active_creature_anim[slot].timeBetweenFidgets = creature->animation.time_between_fidgets();
		active_creature_anim[slot].walkAnimationTime = creature->animation.walk_animation_time();
		active_creature_anim[slot].attackAnimationTime = creature->animation.attack_animation_time();
		active_creature_anim[slot].flightAnimationTime = creature->animation.fight_animation_distance();
	}
}

h3::H3ObjectAttributes ObjectAttributes(int type, int subtype, Appearance const& appearance)
{
	h3::H3ObjectAttributes h3obj;
	h3obj.type = type;
	h3obj.subtype = subtype;

	h3obj.defName.Assign(appearance.def.c_str());

	h3obj.width = appearance.width;
	h3obj.height = appearance.height;

	for (int y = 0; y < 6; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			h3obj.colors(x, y) = false;
			h3obj.passability(x, y) = true;
			h3obj.shadows(x, y) = false;
			h3obj.entrances(x, y) = false;
		}
	}

	for (fvector2 const& v : appearance.colors) h3obj.colors(7 - v.x, 5 - v.y) = true;
	for (fvector2 const& v : appearance.impassability) h3obj.passability(7 - v.x, 5 - v.y) = false;
	for (fvector2 const& v : appearance.shadows) h3obj.shadows(7 - v.x, 5 - v.y) = true;
	for (fvector2 const& v : appearance.entrances) h3obj.entrances(7 - v.x, 5 - v.y) = true;

	h3obj.maskTerrain.bits = 0;

	for (fstring const& terrain_id : appearance.terrains)
	{
		if (terrain_id == ANY_LAND)
		{
			for (Terrain* terrain : game->terrains)
			{
				if (terrain->id != "water" && terrain->id != "rock")
				{
					h3obj.maskTerrain.bitfield[terrain->native_slot] = true;
				}
			}
		}
		else
		{
			Terrain* terrain = game->terrains.by_id[terrain_id];
			h3obj.maskTerrain.bitfield[terrain->native_slot] = true;
		}
	}

	h3obj.flat = appearance.is_flat;
	h3obj.defIndex = 105;
	return h3obj;
}

void UpdateObjects()
{
	auto object_lists = h3::H3Main::Get()->mainSetup.objectLists;
	for (int i = 0; i < OBJECT_TYPES; i++)
	{
		object_lists[i].RemoveAll();
	}

	for (Town* town : game->towns)
	{
		if (!town->appearance.def.is_empty())
		{
			object_lists[OBJECT_TYPE_TOWN].Add(ObjectAttributes(OBJECT_TYPE_TOWN, town->native_slot, town->appearance));
		}
	}

	for (Creature* creature : game->creatures)
	{
		if (!creature->monster_def.is_empty())
		{
			Appearance monster_appearance;
			monster_appearance.def = creature->monster_def;
			monster_appearance.width = 2;
			monster_appearance.height = 2;
			monster_appearance.colors.add(fvector2(0, 0));
			monster_appearance.colors.add(fvector2(1, 0));
			monster_appearance.colors.add(fvector2(0, 1));
			monster_appearance.colors.add(fvector2(1, 1));
			monster_appearance.shadows.add(fvector2(0, 0));
			monster_appearance.shadows.add(fvector2(1, 0));
			monster_appearance.shadows.add(fvector2(0, 1));
			monster_appearance.shadows.add(fvector2(1, 1));
			monster_appearance.impassability.add(fvector2(0, 0));
			monster_appearance.entrances.add(fvector2(0, 0));

			object_lists[OBJECT_TYPE_MONSTER].Add(ObjectAttributes(OBJECT_TYPE_MONSTER, creature->native_slot, monster_appearance));
		}
	}

	for (UnsortedMapObject const& obj : game->unsorted_map_objects)
	{
		object_lists[obj.type].Add(ObjectAttributes(obj.type, obj.subtype, obj.appearance));
	}
}

void UpdateAll()
{
	UpdateCreatures();
	UpdateObjects();
}

Creature* GetRandomCreature(int level_low = 0, int level_high = CREATURE_LEVELS - 1)
{
	flist<Creature*> available_creatures;

	for (Creature* creature : game->creatures)
	{
		if (creature->is_allowed_in_random && creature->level >= level_low && creature->level <= level_high)
		{
			available_creatures.add(creature);
		}
	}

	if (!available_creatures.is_empty())
	{
		return available_creatures[Random(available_creatures.count())];
	}
	else if (level_low > 0 || level_high < CREATURE_LEVELS - 1)
	{
		return GetRandomCreature(level_low - 1, level_high + 1);
	}
	else
	{
		return &fallback_creature;
	}
}

int __stdcall Hook_GetRandomCreature(HiHook* hook, void* game_manager, int level_low, int level_high)
{
	return GetRandomCreature(level_low, level_high)->native_slot;
}

int GetSkeltransCreature(int original_creature)
{
	Creature* creature = game->creatures.by_native_slot[original_creature];
	if (creature == nullptr || creature->skeltrans_immune) return original_creature;

	Creature* new_creature = game->creatures.by_id[creature->skeltrans_dragon ? "creature_bone_dragon" : creature->skeltrans_knight ? "creature_black_knight" : "creature_skeleton"];
	if (new_creature == nullptr) return original_creature;

	return new_creature->native_slot;
}

struct TownNamesManager
{
	std::map<int, flist<fstring>> free_names;

	void Reset()
	{
		free_names.clear();
		for (Town* town : game->towns)
		{
			for (int i = 0; i < town->town_object_names.count(); i++)
			{
				free_names[town->native_slot].add(town->town_object_names[i]);
			}
		}
		if (free_names.empty())
		{
			free_names[-1].add("Town");
		}
	}

	fstring GetRandomName(int town_rn)
	{
		if (free_names.contains(town_rn))
		{
			int index = Random(free_names[town_rn].count());
			fstring name = free_names[town_rn][index];
			free_names[town_rn].remove_at(index);
			if (free_names[town_rn].is_empty())
			{
				free_names.erase(town_rn);
			}
			return name;
		}
		else if (free_names.size() > 0)
		{
			int town_index = Random(free_names.size());
			auto it = free_names.begin();
			for (int i = 0; i < town_index; i++)
			{
				it++;
			}
			return GetRandomName(it->first);
		}
		else
		{
			Reset();
			return GetRandomName(town_rn);
		}
	}
};

TownNamesManager town_names_manager;


void ParseDefFrame(fstring const& value, h3::H3LoadedDef*& def, int& frame)
{
	int colon_pos = value.find(':');
	fstring def_name = value.substring(0, colon_pos);
	frame = value.substring(colon_pos + 1).parse_int();
	Asset* asset = all_assets.by_id[def_name];
	fstring h3name = fstring("pw") + std::to_string(asset->native_slot) + ".def";

	if (asset != nullptr && def->GetName() != h3name)
	{
		auto resource_manager = h3::H3ResourceManager::Get();
		auto node = resource_manager->FindItem(h3name.c_str());
		if (node != nullptr)
		{
			def = static_cast<h3::H3LoadedDef*>(node->data.m_item);
		}
		else
		{
			def = h3::H3LoadedDef::Load(h3name.c_str());
		}
	}
}

void RedirectDefFrame(h3::H3LoadedDef*& def, int& frame)
{
	fstring name = fstring(def->GetName()).lower();

	if (name == "twcrport.def" && frame >= 2)
	{
		Creature* creature = game->creatures.by_native_slot[frame - 2];
		if (creature == nullptr) creature = &fallback_creature;

		ParseDefFrame(creature->interface_def, def, frame);
	}
	if (name == "cprsmall.def" && frame >= 2)
	{
		Creature* creature = game->creatures.by_native_slot[frame - 2];
		if (creature == nullptr) creature = &fallback_creature;

		ParseDefFrame(creature->interface_def_small, def, frame);
	}
	if (name == "avwattak.def")
	{
		Creature* creature = game->creatures.by_native_slot[frame / 2];
		if (creature == nullptr) creature = &fallback_creature;

		int remainder = frame % 2;
		ParseDefFrame(creature->monster_attack_def, def, frame);
		frame += remainder;
	}
}

int __stdcall Hook_DrawInterfaceDefFrame(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef* def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror);
}

int __stdcall Hook_DrawInterfaceDefButtonFrame(HiHook* hook, h3::H3LoadedDef* def, int frame, 
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def, int frame,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror)>(hook->GetDefaultFunc())(def, frame,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror);
}

int __stdcall Hook_DrawInterfaceDefGroupFrame(HiHook* hook, h3::H3LoadedDef* def,
	int group, int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror, bool use_palette_special_colors)
{
	if (group == 0) RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef* def,
		int group, int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror, bool use_palette_special_colors)>(hook->GetDefaultFunc())(def,
			group, frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror, use_palette_special_colors);
}

int __stdcall Hook_DrawInterfaceDefGroupFrameWithOptHalfTransp(HiHook* hook, h3::H3LoadedDef* def,
	int group, int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror, bool use_palette_special_colors)
{
	if (group == 0) RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef* def,
		int group, int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror, bool use_palette_special_colors)>(hook->GetDefaultFunc())(def,
			group, frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror, use_palette_special_colors);
}

int __stdcall Hook_DrawDefFrameType3FlagObject(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, short flag_color, bool hmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, short flag_color, bool hmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, flag_color, hmirror);
}

int __stdcall Hook_DrawDefFrameType3NotFlagObject(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror);
}

int __stdcall Hook_DrawDefFrameType3Shadow(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror);
}

int __stdcall Hook_DrawDefFrameType0Or2(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror, bool vmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror, bool vmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror, vmirror);
}

int __stdcall Hook_DrawDefFrameType0Or2Shadow(HiHook* hook, h3::H3LoadedDef* def,
	int frame, int src_x, int src_y, int src_width, int src_height,
	void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height, int scanline_size, bool hmirror, bool vmirror)
{
	RedirectDefFrame(def, frame);
	return reinterpret_cast<int (__thiscall*)(h3::H3LoadedDef * def,
		int frame, int src_x, int src_y, int src_width, int src_height,
		void* draw_buffer, int dest_x, int dest_y, int dest_width, int dest_height,
		int scanline_size, bool hmirror, bool vmirror)>(hook->GetDefaultFunc())(def,
			frame, src_x, src_y, src_width, src_height,
			draw_buffer, dest_x, dest_y, dest_width, dest_height, scanline_size, hmirror, vmirror);
}


void cmp(u32 x, u32 y, HookContext* context)
{
	context->flags.ZF = x == y;
	context->flags.CF = x < y;
}

Terrain* get_or_use_first(fref<Terrain> const& ref)
{
	Terrain* terrain = ref;
	if (terrain != nullptr) return terrain;
	else return game->terrains.by_pos[0];
}

struct Gamehooks : public Hooks
{
	LOHOOK(0x5849BD) { exec = false; eax = GetMapVersionDefCadre(eax); }
	LOHOOK(0x5849C2) { exec = false; }
	LOHOOK(0x5849C7) { exec = false; }
	LOHOOK(0x5849CC) { exec = false; }

	// generated map description
	LOHOOK(0x54A302) { exec = false; edi = fstring(", pow expansion map"); }

	LOHOOK(0x43D4F2) { exec = false; if (edx != -1) edx = get_or_use_first(game->towns.by_native_slot[edx]->native_terrain)->native_slot; }
	LOHOOK(0x44C2AE) { exec = false; if (ecx != -1) ecx = get_or_use_first(game->towns.by_native_slot[ecx]->native_terrain)->native_slot; }
	LOHOOK(0x5C1844) { exec = false; eax = get_or_use_first(game->towns.by_native_slot[eax]->native_terrain)->native_slot; }
	LOHOOK(0x532F9E) { exec = false; eax = get_or_use_first(game->towns.by_native_slot[eax]->rmg_terrain)->native_slot; }

	LOHOOK(0x5C70F2) { exec = false; eax = game->towns.by_native_slot[edx]->mp3; }

	LOHOOK(0x40C9A8) { exec = false; ecx = game->towns.by_native_slot[ecx]->name; }
	LOHOOK(0x5697F7) { exec = false; ecx = game->towns.by_native_slot[eax]->name; }
	LOHOOK(0x57635A) { exec = false; ecx = game->towns.by_native_slot[edi]->name; }
	LOHOOK(0x58D95A) { exec = false; ecx = game->towns.by_native_slot[eax]->name; }
	LOHOOK(0x58DD24) { exec = false; ecx = game->towns.by_native_slot[eax]->name; }
	LOHOOK(0x5C1854) { exec = false; eax = game->towns.by_native_slot[eax]->name; }
	LOHOOK(0x54A4AA) { exec = false; edi = game->towns.by_native_slot[eax]->rmg_name_lowercase; }

	LOHOOK(0x4CA756) { town_names_manager.Reset(); }
	LOHOOK(0x4CA994) { exec = false; eax = 0; esi = town_names_manager.GetRandomName(esi); }
	LOHOOK(0x4CA9C0) { exec = false; }
	LOHOOK(0x4CA9D0) { exec = false; }

	LOHOOK(0x52C9CA) { exec = false; edx = game->towns.by_native_slot[eax]->puzzle.pieces_pcx_part; }
	LOHOOK(0x52CF59) { exec = false; edx = game->towns.by_native_slot[esi]->puzzle.pieces_pcx_part; }

	// todo: pieces x, y, order

	LOHOOK(0x4C9820) { exec = false; edi = game->towns.by_native_slot[ecx]->map_def_village; }
	LOHOOK(0x4C980D) { exec = false; edi = game->towns.by_native_slot[ecx]->map_def_castle; }
	LOHOOK(0x4C97C1) { exec = false; edi = game->towns.by_native_slot[eax]->map_def_capitol; }

	LOHOOK(0x5664B5) { exec = false; eax = GetSkeltransCreature(ecx); }
	LOHOOK(0x56689D) { exec = false; cmp(edx, GetSkeltransCreature(edx), context); }
	LOHOOK(0x566D79) { exec = false; cmp(edx, GetSkeltransCreature(edx), context); }
	LOHOOK(0x566F3C) { exec = false; cmp(esi, GetSkeltransCreature(esi), context); }
	LOHOOK(0x566FA4) { exec = false; eax = GetSkeltransCreature(esi); }
	LOHOOK(0x566FD1) { exec = false; cmp(edx, GetSkeltransCreature(edx), context); }

	LOHOOK(0x43DA91) { ecx = game->creatures.by_native_slot[eax]->missle_def; eax = 200; }
};

#pragma pack(push, 1)
struct GamehooksTownObjectNames
{
	static constexpr int COUNT = 16;
	char const* names[COUNT];
};

struct GamehooksPuzzleComponent
{
	s16 value[PUZZLE_PIECES];
};

struct GamehooksMissileJump
{
	u8 _b9;
	char const* def_name;
};
#pragma pack(pop)


void InitializeGamehooks()
{
	if (exe_type != ExeType::main) return;

	fstring noisy_patchers[] = { "HD.TE", "HD.Af", "HD+", "HD.Suits" };
	for (auto& noisy_patcher_name : noisy_patchers)
	{
		auto noisy_patcher = patchers->GetInstance(noisy_patcher_name.c_str());
		if (noisy_patcher != nullptr)
		{
			noisy_patcher->UndoAll();
		}
	}

	// no async sounds
	s32 async_patches[] = {
		0x445EB9, 0x445FE8, 0x4461E7, 0x462C65, 0x59A1C0, 0x59A710, 0x59A770, 0x59A7C0, 0x5A067E, 0x5A071A,
		0x5A07E1, 0x5A0814, 0x5A088C, 0x5A0934, 0x5A0A03, 0x5A0A37,
	};
	for (s32 address : async_patches) patchers->UndoAllAt(address);
	auto patch_6416E4 = patchers->GetLastPatchAt(0x6416E4);
	if (patch_6416E4 != nullptr) patch_6416E4->Undo();

	// dark transitions
	//patchers->UndoAllAt(0x603210);
	//auto patch_6038A0 = patchers->GetLastPatchAt(0x6038A0);
	//if (patch_6038A0 != nullptr && fstring(patch_6038A0->GetOwner()) != "HD.True32") patch_6038A0->Undo();

	// original spell book
	s32 spell_book_patches[] = {
		0x59C0F0, 0x59C21E, 0x59C260, 0x59C291, 0x59C298, 0x59C29D, 0x59C2F4, 0x59C2F7, 0x59C304, 0x59C30E,
		0x59C315, 0x59C31A, 0x59C371, 0x59C374, 0x59C381, 0x59C3A2, 0x59C3A9, 0x59C3AE, 0x59C408, 0x59C40B,
		0x59C418, 0x59C422, 0x59C429, 0x59C42E, 0x59C488, 0x59C48B, 0x59C498, 0x59C4B9, 0x59C4C0, 0x59C4C5,
		0x59C4F6, 0x59C51F, 0x59C522, 0x59C52F, 0x59C539, 0x59C540, 0x59C545, 0x59C576, 0x59C59F, 0x59C5A2,
		0x59C5AF, 0x59CA0D, 0x59CA19, 0x59CA1B, 0x59CA20, 0x59CA25, 0x59CE9C, 0x59CF85, 0x59CFB7, 0x59CFBD,
		0x59CFC0, 0x59CFC3, 0x59CFC6, 0x59D24B, 0x59D24D, 0x59D24E, 0x59D24F, 0x59D256, 0x59D269, 0x59D273,
		0x59D2A9, 0x59D2AF, 0x59D2B2, 0x59D2B7, 0x59D396, 0x59D39E, 0x59D3A6, 0x59D3A8, 0x59D3AB, 0x59D4AE,
		0x59D4E0, 0x59D537, 0x59D5B6, 0x59D65F, 0x59D681, 0x59D6B6, 0x59D6B8, 0x59D6B9, 0x59D6BE, 0x59D6C3,
		0x59D6C9, 0x59D6CD, 0x59D6D3, 0x59D6D6, 0x59D6DB, 0x59D6DD, 0x59D6E2, 0x59D6E4, 0x59D6E6, 0x59D6EB,
		0x59D6F1, 0x59D715, 0x59D71D, 0x59D720, 0x59D722, 0x59D724, 0x59D728, 0x59D72A, 0x59D72D, 0x59D72F,
		0x59D733, 0x59D735, 0x59D738, 0x59D73B, 0x59D73E, 0x59D73F, 0x59D744, 0x59D747, 0x59D74B, 0x59D74D,
		0x59D74F, 0x59D752, 0x59D839, 0x59D8B0, 0x59D8F8, 0x59D940, 0x59D99A, 0x59DA19, 0x59DA25, 0x59DA2F,
		0x59DA30, 0x59DA33,
	};
	for (s32 address : spell_book_patches) patchers->UndoAllAt(address);

	patcher->WriteDword(0x67F554, 3); // делает вид, что установлен AB

	if (exe_version == ExeVersion::pow)
	{
		game = new Game();
		game->initialize();

		std::ifstream list_file(rules_dir / "list.txt");
		std::string line;
		while (std::getline(list_file, line))
		{
			if (!line.empty())
			{
				jobject jrule = jobject::load_fjs(rules_dir / (line + ".pwr"));
				game->jpatch(jobject(J_MERGE_ASSIGN, jrule));
			}
		}

		UpdateAll();

		Gamehooks gamehooks;
		gamehooks.Write();

		// show all map versions (todo: fix)
		patcher->WriteByte(0x4C415E, 0x73);
		patcher->WriteByte(0x513791, 0x73);
		patcher->WriteByte(0x5836DF, 0x73);
		patcher->WriteByte(0x585BD7, 0x73);
		patcher->WriteByte(0x58C168, 0x73);
		patcher->WriteByte(0x502BA3, 0x0F);
		patcher->WriteByte(0x502BA4, 0x93);

		// map version in generated maps
		patcher->WriteByte(0x54A1D5, (int)MapVersion::pow);

		patcher->WriteDword(0x6747B0, (int)active_creature_info);
		patcher->WriteDword(0x67FF74, (int)active_creature_anim);

		s32 checks_for_valid_creature[] = {
			0x40AC91 + 1, 0x40ACF1 + 2, 0x40C2B0 + 1, 0x416948 + 1, 0x43F9B2 + 1, 0x43FA88 + 1, 0x43FDA8 + 1, 0x43FE24 + 2, 0x4401B5 + 1, 0x4409D7 + 1,
			0x440A21 + 1, 0x440A85 + 1, 0x440C9A + 1, 0x440CD5 + 1, 0x440D49 + 1, 0x440D86 + 1, 0x440F5B + 1, 0x4418D0 + 1, 0x44192E + 1, 0x443265 + 1,
			0x44329A + 1, 0x443307 + 1, 0x44333C + 1, 0x44372A + 2, 0x443796 + 2, 0x446C86 + 1, 0x446CF4 + 1, 0x44730E + 1, 0x44BB13 + 2, 0x44BC64 + 1,
			0x44C060 + 2, 0x464670 + 1, 0x46487A + 1, 0x464A91 + 1, 0x465242 + 1, 0x4652E4 + 1, 0x478593 + 1, 0x4785CB + 1, 0x4790D7 + 1, 0x479142 + 1,
			0x47927F + 1, 0x47931B + 1, 0x4840B2 + 1, 0x4863FC + 2, 0x491F65 + 1, 0x491FB5 + 1, 0x4920A9 + 2, 0x4921A1 + 2, 0x4921EC + 2, 0x4922B6 + 1,
			0x4922ED + 1, 0x49233E + 1, 0x492375 + 1, 0x492521 + 1, 0x4925BE + 1, 0x49262F + 1, 0x4926C9 + 1, 0x49275D + 1, 0x4927E8 + 1, 0x492959 + 1,
			0x492D0A + 2, 0x492DAD + 1, 0x49E705 + 2, 0x49E728 + 2, 0x49EE73 + 1, 0x49EFC0 + 1, 0x4A0435 + 2, 0x4A04D7 + 2, 0x4A1655 + 2, 0x4A17D9 + 1,
			0x4A1803 + 1, 0x4A1828 + 1, 0x4A184D + 1, 0x4A189B + 1, 0x4A19FA + 2, 0x4A1A9F + 2, 0x4A42F1 + 1, 0x4A6A7E + 1, 0x4A6C96 + 2, 0x4A6F57 + 2,
			0x4A6FC6 + 1, 0x4AB893 + 2, 0x4ABC6C + 1, 0x4ABF68 + 1, 0x4AE27F + 1, 0x4AE2CC + 2, 0x4CC86D + 1, 0x4CC8C9 + 1, 0x4CC988 + 1, 0x4DBA61 + 1,
			0x4DBA9D + 2, 0x4DBAE0 + 2, 0x4DBB2D + 1, 0x4DBB74 + 1, 0x4DBB96 + 2, 0x4F1F88 + 1, 0x4F2010 + 1, 0x4F20D6 + 1, 0x4F2138 + 1, 0x4F479B + 1,
			0x4F5BD0 + 2, 0x4F5C4A + 2, 0x51402E + 1, 0x514078 + 1, 0x5213E4 + 2, 0x52141C + 2, 0x5217C7 + 1, 0x521812 + 1, 0x521835 + 2, 0x52191C + 1,
			0x52195C + 1, 0x5219BD + 1, 0x5219CD + 2, 0x5219F7 + 2, 0x52FEF7 + 2, 0x52FF5B + 2, 0x52FFB9 + 2, 0x55036E + 1, 0x550825 + 1, 0x55113E + 1,
			0x551D8E + 2, 0x5632DA + 2, 0x565365 + 1, 0x56637B + 1, 0x5664C8 + 1, 0x56EBD3 + 1, 0x56ECD4 + 1, 0x56F115 + 1, 0x5706A5 + 1, 0x570A6E + 1,
			0x570D4D + 1, 0x59F90A + 2, 0x5A21ED + 1, 0x5A22E6 + 1, 0x5A2CC3 + 1, 0x5A2DCD + 2, 0x5A2E27 + 2, 0x5A753A + 1, 0x5A77C0 + 2, 0x5A77FB + 2,
			0x5A79B2 + 1, 0x5A79ED + 1, 0x5A7D4B + 2, 0x5A7DBB + 2, 0x5A8A08 + 2, 0x5A8A69 + 2, 0x5A8B52 + 2, 0x5A9378 + 1, 0x5B0E09 + 2, 0x5C08EA + 2,
			0x5C605C + 1, 0x5C656A + 1, 0x5C789A + 1, 0x5C7946 + 1, 0x5C79BC + 1, 0x5C7A1D + 1, 0x5C7AB0 + 1, 0x5C7B25 + 2, 0x5C7B48 + 1, 0x5C7FD4 + 1,
			0x5C8046 + 1, 0x5C84B0 + 1, 0x5C9C3A + 1, 0x5D0C56 + 1, 0x5D1059 + 2, 0x5DD0A4 + 1, 0x5DD976 + 1, 0x5DDA4A + 1, 0x5EF08C + 1, 0x5F3939 + 1,
			0x5F40CC + 1,
		};
		for (s32 address : checks_for_valid_creature) patcher->WriteDword(address, MAX_CREATURES);

		patcher->WriteHiHook(0x4C8F80, SPLICE_, EXTENDED_, THISCALL_, Hook_GetRandomCreature);

		// 487980: read campaign scenario, todo: remove bitmasks
		// bitmasks usage: 0x4CF73C, 0x4CF7AA, 0x48E61F

		// 541010: rmg monster generation, todo: remove array on stack
		// number of creatures: 0x541064, 0x5410B4

		// fix с наймом АИ существ в замке, если они >145
		patcher->WriteDword(0x428885 + 1, MAX_CREATURES * 12);
		patcher->WriteDword(0x4288C4 + 2, MAX_CREATURES * 12);
		patcher->WriteDword(0x4288A3 + 3, MAX_CREATURES);
		patcher->WriteDword(0x4288E3 + 1, MAX_CREATURES);

		// возможность апгрейда существ в неродном городе
		patcher->WriteByte(0x4C698F, 0x3B);
		patcher->WriteByte(0x4C6990, 0xC0);

		// missile def hook
		patcher->WriteByte(0x43DB1A, 0x90);
		patcher->WriteByte(0x43DB1B, 0x90);
		patcher->WriteByte(0x43DB1C, 0x90);

		patcher->WriteHiHook(0x47B820, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawInterfaceDefFrame);
		patcher->WriteHiHook(0x47B7D0, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawInterfaceDefButtonFrame);
		patcher->WriteHiHook(0x47B610, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawInterfaceDefGroupFrame);
		patcher->WriteHiHook(0x47BA90, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawInterfaceDefGroupFrameWithOptHalfTransp);
		patcher->WriteHiHook(0x47B730, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawDefFrameType3FlagObject);
		patcher->WriteHiHook(0x47B6E0, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawDefFrameType3NotFlagObject);
		patcher->WriteHiHook(0x47B780, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawDefFrameType3Shadow);
		patcher->WriteHiHook(0x47B870, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawDefFrameType0Or2);
		patcher->WriteHiHook(0x47B8C0, SPLICE_, EXTENDED_, THISCALL_, Hook_DrawDefFrameType0Or2Shadow);
	}

	patchers->SaveDump("patcher_dump.txt");
	std::string dump_file_str;
	if (true)
	{
		std::ifstream dump_file_in("patcher_dump.txt");
		std::string line;
		while (std::getline(dump_file_in, line))
		{
			if (line != "[ ][ ][ ] 0: \r")
			{
				while (true)
				{
					int pos = line.find(" 00000");
					if (pos == std::string::npos) break;
					line = line.substr(0, pos) + line.substr(pos + 11);
				}
				dump_file_str += line;
			}
		}
	}
	if (true)
	{
		std::ofstream dump_file_out("patcher_dump.txt");
		dump_file_out << dump_file_str;
	}
}

void DumpGameRuleset()
{
	if (exe_type != ExeType::main) return;

	if (exe_version != ExeVersion::pow)
	{
		game = new Game();
		game->initialize();

		for (fstring const& id : game->towns.fixed_native_slots) { if (!id.is_empty()) game->towns.create(id); }
		for (fstring const& id : game->terrains.fixed_native_slots) { if (!id.is_empty()) game->terrains.create(id); }
		for (fstring const& id : game->resources.fixed_native_slots) { if (!id.is_empty()) game->resources.create(id); }
		for (fstring const& id : game->creatures.fixed_native_slots) { if (!id.is_empty()) game->creatures.create(id); }

		//int num_creatures = *reinterpret_cast<int*>(0x40AC91 + 1);

		auto town_native_terrain = ARRAY_AT_ADDRESS(s32, 0x6436A8);
		auto town_rmg_terrain = ARRAY_AT_ADDRESS(s32, 0x6408D8);

		auto town_mp3 = ARRAY_AT_ADDRESS(char const*, 0x6436CC);

		auto town_name = ARRAY_AT_ADDRESS(char const*, 0x6A755C);
		auto town_rmg_name_lowercase = ARRAY_AT_ADDRESS(char const*, 0x68277C);

		auto town_object_names = ARRAY_AT_ADDRESS(GamehooksTownObjectNames, 0x6A60B0);

		auto town_puzzle_pieces_pcx_part = ARRAY_AT_ADDRESS(char const*, 0x6818D0);
		auto town_puzzle_pieces_x = ARRAY_AT_ADDRESS(GamehooksPuzzleComponent, 0x6818F4);
		auto town_puzzle_pieces_y = ARRAY_AT_ADDRESS(GamehooksPuzzleComponent, 0x681954);
		auto town_puzzle_pieces_order = ARRAY_AT_ADDRESS(GamehooksPuzzleComponent, 0x681FB4);

		auto town_map_def_village = ARRAY_AT_ADDRESS(char const*, 0x677A0C);
		auto town_map_def_castle = ARRAY_AT_ADDRESS(char const*, 0x677A30);
		auto town_map_def_capitol = ARRAY_AT_ADDRESS(char const*, 0x677A54);

		auto creature_info = ARRAY_AT_PTR(h3::H3CreatureInformation, 0x6747B0);
		auto creature_anim = ARRAY_AT_PTR(h3::H3CreatureAnimation, 0x67FF74);

		auto creature_skeltrans = ARRAY_AT_PTR(s32, 0x5664B5 + 3);

		auto creature_missle_jumper = ARRAY_AT_PTR(s8, 0x43DA9D + 2);
		auto creature_missle_jumptable = ARRAY_AT_ADDRESS(GamehooksMissileJump*, 0x43DB60);

		for (Town* town : game->towns)
		{
			if (exe_version == ExeVersion::hota) break;

			int slot = town->native_slot;

			town->native_terrain = game->terrains.by_native_slot[town_native_terrain[slot]];
			town->rmg_terrain = game->terrains.by_native_slot[town_rmg_terrain[slot]];

			town->mp3 = town_mp3[slot];

			town->name = town_name[slot];
			town->rmg_name_lowercase = town_rmg_name_lowercase[slot];

			for (int i = 0; i < GamehooksTownObjectNames::COUNT; i++)
			{
				town->town_object_names.add(town_object_names[slot].names[i]);
			}

			town->puzzle.pieces_pcx_part = town_puzzle_pieces_pcx_part[slot];

			for (int i = 0; i < PUZZLE_PIECES; i++)
			{
				town->puzzle.pieces_x[i] = town_puzzle_pieces_x[slot].value[i];
				town->puzzle.pieces_y[i] = town_puzzle_pieces_y[slot].value[i];
				town->puzzle.pieces_order[i] = town_puzzle_pieces_order[slot].value[i];
			}

			town->map_def_village = town_map_def_village[slot];
			town->map_def_castle = town_map_def_castle[slot];
			town->map_def_capitol = town_map_def_capitol[slot];
		}

		for (Creature* creature : game->creatures)
		{
			int slot = creature->native_slot;

			if (!creature_info[slot].siegeWeapon && creature_info[slot].advMapHigh > 0 && !creature->id.starts_with("wog_npc") && creature->id != "hota_steel_golem")
			{
				creature->is_allowed_in_editor = true;
				creature->is_allowed_in_random = true;
			}

			creature->native_town = game->towns.by_native_slot[creature_info[slot].town];
			creature->level = creature_info[slot].level;
			creature->sound_prefix = creature_info[slot].soundName;
			creature->battle_def = creature_info[slot].defName;

			if (exe_version == ExeVersion::hota)
			{
				if (creature->id == "hota_cannon") creature->missle_def = "smcanx.def";
				else if (creature->id == "hota_pirate") creature->missle_def = "ppirate.def";
				else if (creature->id == "hota_corsair") creature->missle_def = "ppirate.def";
				else if (creature->id == "hota_sea_dog") creature->missle_def = "ppirate.def";
				else if (creature->id == "hota_sea_witch") creature->missle_def = "psorc.def";
				else if (creature->id == "hota_sorceress") creature->missle_def = "psorc.def";
			}
			else if (slot >= 2)
			{
				if (slot <= 146 || (exe_version == ExeVersion::wog || exe_version == ExeVersion::era))
				{
					s8 jump_index = creature_missle_jumper[slot - 2];
					if (jump_index < 0x10)
					{
						if (jump_index == 0 && (exe_version == ExeVersion::wog || exe_version == ExeVersion::era))
						{
							if (slot == 196) creature->missle_def = "ZShot195.def";
							else creature->missle_def = "plcbowx.def";
						}
						else
						{
							creature->missle_def = creature_missle_jumptable[jump_index]->def_name;
						}
					}
				}
			}

			if (slot < 145)
			{
				creature->interface_def = fstring("sod_twcrport.def:") + std::to_string(slot + 2);
				creature->interface_def_small = fstring("sod_cprsmall.def:") + std::to_string(slot + 2);
				creature->monster_attack_def = fstring("sod_avwattak.def:") + std::to_string(slot * 2);
			}
			else if (slot < 149)
			{
				creature->interface_def = fstring("sod_twcrport.def:") + std::to_string(slot + 2);
				creature->interface_def_small = fstring("sod_cprsmall.def:") + std::to_string(slot + 2);
				creature->monster_attack_def = fstring("avwattak.def:") + std::to_string(slot * 2);
			}
			else if (slot == 149)
			{
				creature->interface_def = fstring("twcrport.def:") + std::to_string(slot + 2);
				creature->interface_def_small = fstring("cprsmall.def:") + std::to_string(slot + 2);
				creature->monster_attack_def = fstring("avwattak.def:") + std::to_string(slot * 2);
			}
			else if (exe_version == ExeVersion::hota)
			{
				creature->interface_def = fstring("hota_twcrport.def:") + std::to_string(slot + 2);
				creature->interface_def_small = fstring("hota_cprsmall.def:") + std::to_string(slot + 2);
				creature->monster_attack_def = fstring("hota_avwattak.def:") + std::to_string(slot * 2);
			}
			else if (exe_version == ExeVersion::wog || exe_version == ExeVersion::era)
			{
				creature->interface_def = fstring("wog_twcrport.def:") + std::to_string(slot + 2);
				creature->interface_def_small = fstring("wog_cprsmall.def:") + std::to_string(slot + 2);
				creature->monster_attack_def = fstring("wog_avwattak.def:") + std::to_string(slot * 2);
			}

			creature->double_wide = creature_info[slot].doubleWide;
			creature->flying = creature_info[slot].flyer;
			creature->shooting = creature_info[slot].shooter;
			creature->dragon_breath = creature_info[slot].extendedAttack;
			creature->is_alive = creature_info[slot].alive;
			creature->can_attack_walls = creature_info[slot].destroyWalls;
			creature->is_siege_weapon = creature_info[slot].siegeWeapon;

			if (creature_info[slot].king1 && creature_info[slot].king2) throw std::exception();
			if (creature_info[slot].king1 && creature_info[slot].king3) throw std::exception();
			if (creature_info[slot].king2 && creature_info[slot].king3) throw std::exception();

			if (creature_info[slot].king1) creature->king_level = 1;
			if (creature_info[slot].king2) creature->king_level = 2;
			if (creature_info[slot].king3) creature->king_level = 3;

			creature->mind_immunity = creature_info[slot].mindImmunity;
			creature->shooting_ray = creature_info[slot].shootsRay;
			creature->no_melee_penalty = creature_info[slot].noMeleePenalty;

			if (creature_info[slot].unk2000) throw std::exception();

			creature->fire_immunity = creature_info[slot].fireImmunity;
			creature->attack_twice = creature_info[slot].doubleAttack;
			creature->no_retaliation = creature_info[slot].noRetaliation;
			creature->no_morale = creature_info[slot].noMorale;
			creature->is_undead = creature_info[slot].undead;
			creature->attack_all_around = creature_info[slot].attackAllAround;
			creature->fireball_attack = creature_info[slot].fireballAttack;

			if (creature_info[slot].cannotMove) throw std::exception();
			if (creature_info[slot].summon) throw std::exception();
			if (creature_info[slot].clone) throw std::exception();
			if (creature_info[slot].morale) throw std::exception();
			if (creature_info[slot].waiting) throw std::exception();
			if (creature_info[slot].done) throw std::exception();
			if (creature_info[slot].defending) throw std::exception();
			if (creature_info[slot].sacrificed) throw std::exception();
			if (creature_info[slot].noColoring) throw std::exception();
			if (creature_info[slot].gray) throw std::exception();

			creature->is_dragon = creature_info[slot].dragon;

			creature->name_singular = creature_info[slot].nameSingular;
			creature->name_plural = creature_info[slot].namePlural;
			creature->description = creature_info[slot].description;

			creature->cost["wood"] = creature_info[slot].cost.wood;
			creature->cost["mercury"] = creature_info[slot].cost.mercury;
			creature->cost["ore"] = creature_info[slot].cost.ore;
			creature->cost["sulfur"] = creature_info[slot].cost.sulfur;
			creature->cost["crystal"] = creature_info[slot].cost.crystal;
			creature->cost["gems"] = creature_info[slot].cost.gems;
			creature->cost["gold"] = creature_info[slot].cost.gold;

			creature->fight_value = creature_info[slot].fightValue;
			creature->ai_value = creature_info[slot].aiValue;
			creature->growth = creature_info[slot].grow;
			creature->horde_growth = creature_info[slot].hGrow;
			creature->hit_points = creature_info[slot].hitPoints;
			creature->speed = creature_info[slot].speed;
			creature->attack = creature_info[slot].attack;
			creature->defence = creature_info[slot].defence;
			creature->damage_low = creature_info[slot].damageLow;
			creature->damage_high = creature_info[slot].damageHigh;
			creature->num_shots = creature_info[slot].numberShots;
			creature->num_spells = creature_info[slot].spellCharges;
			creature->adv_map_low = creature_info[slot].advMapLow;
			creature->adv_map_high = creature_info[slot].advMapHigh;

			for (int i = 0; i < 3; i++)
			{
				creature->animation.missile_offsets[i].x = creature_anim[slot].missiles[i].offset_x;
				creature->animation.missile_offsets[i].y = creature_anim[slot].missiles[i].offset_y;
			}

			for (int i = 0; i < 12; i++)
			{
				creature->animation.missile_frame_angles[i] = creature_anim[slot].missileFrameAngles[i];
			}

			creature->animation.troop_count_location_offset = creature_anim[slot].troopCountLocationOffset;
			creature->animation.attack_climax_frame = creature_anim[slot].attackClimaxFrame;
			creature->animation.time_between_fidgets_multiplier = 1.0f * creature_anim[slot].timeBetweenFidgets / creature->animation.time_between_fidgets();
			creature->animation.walk_animation_time_multiplier = 1.0f * creature_anim[slot].walkAnimationTime / creature->animation.walk_animation_time();
			creature->animation.attack_animation_time_multiplier = 1.0f * creature_anim[slot].attackAnimationTime / creature->animation.attack_animation_time();
			creature->animation.fight_animation_distance_multiplier = 1.0f * creature_anim[slot].flightAnimationTime / creature->animation.fight_animation_distance();

			if (creature->is_siege_weapon || creature_skeltrans[slot] == slot) creature->skeltrans_immune = true;
			else if (creature_skeltrans[slot] == game->creatures.by_id["creature_bone_dragon"]->native_slot) creature->skeltrans_dragon = true;
			else if (creature_skeltrans[slot] == game->creatures.by_id["creature_black_knight"]->native_slot) creature->skeltrans_knight = true;
			else if (creature_skeltrans[slot] != game->creatures.by_id["creature_skeleton"]->native_slot) throw std::exception();
		}

		auto object_lists = h3::H3Main::Get()->mainSetup.objectLists;
		for (int i = 0; i < OBJECT_TYPES; i++)
		{
			for (auto& h3obj : object_lists[i])
			{
				if (h3obj.type != i) throw std::exception();

				Appearance* appearance = nullptr;
				bool force_any_land = false;

				if (h3obj.type == OBJECT_TYPE_TOWN)
				{
					appearance = &game->towns.by_native_slot[h3obj.subtype]->appearance;
					force_any_land = true;
				}
				else if (h3obj.type == OBJECT_TYPE_MONSTER)
				{
					game->creatures.by_native_slot[h3obj.subtype]->monster_def = h3obj.defName.String();
					continue;
				}
				else
				{
					auto& obj = game->unsorted_map_objects.add_new();
					obj.type = h3obj.type;
					obj.subtype = h3obj.subtype;
					appearance = &obj.appearance;
				}

				appearance->def = h3obj.defName.String();
				appearance->width = h3obj.width;
				appearance->height = h3obj.height;

				for (int y = 0; y < 6; y++)
				{
					for (int x = 0; x < 8; x++)
					{
						if (h3obj.colors(7 - x, 5 - y)) appearance->colors.add(fvector2(x, y));
						if (!h3obj.passability(7 - x, 5 - y)) appearance->impassability.add(fvector2(x, y));
						if (h3obj.shadows(7 - x, 5 - y)) appearance->shadows.add(fvector2(x, y));
						if (h3obj.entrances(7 - x, 5 - y)) appearance->entrances.add(fvector2(x, y));
					}
				}

				if (!force_any_land)
				{
					bool any_land = true;
					appearance->terrains.clear();
					for (Terrain* terrain : game->terrains)
					{
						if (h3obj.maskTerrain.bitfield[terrain->native_slot])
						{
							appearance->terrains.add(terrain->id);
						}
						else if (terrain->id != "water" && terrain->id != "rock")
						{
							any_land = false;
						}
					}
					if (any_land)
					{
						appearance->terrains.clear();
						appearance->terrains.add(ANY_LAND);

						if (h3obj.maskTerrain.bitfield[game->terrains.by_id["water"]->native_slot])
						{
							appearance->terrains.add("water");
						}
						if (h3obj.maskTerrain.bitfield[game->terrains.by_id["rock"]->native_slot])
						{
							appearance->terrains.add("rock");
						}
					}
				}

				appearance->is_flat = h3obj.flat;

				//if (h3obj.defIndex != 105) throw std::exception(); // todo: why this is not true in era hd
			}
		}

		jobject jgame_base = Game().jsave();
		if (exe_version != ExeVersion::sod)
		{
			jobject jsod = jobject::load_fjs(rules_dir / "sod.pwr");
			Game sod_game;
			sod_game.initialize();
			sod_game.jpatch(jobject(J_MERGE_ASSIGN, jsod));
			jgame_base = sod_game.jsave();
		}
		jobject jgame_diff = game->jdiff(jgame_base);
		jobject jrule(J_BLOCK);
		if (jgame_diff.type == J_MERGE_ASSIGN)
		{
			if (jgame_diff.count() != 1) throw todo_throw_proper_exception();
			jrule = jgame_diff[0];
		}
		else if (jgame_diff.type != J_NONE)
		{
			throw todo_throw_proper_exception();
		}
		jrule.save_fjs(rules_dir / (exe_version_str + ".pwr"));
	}
}
