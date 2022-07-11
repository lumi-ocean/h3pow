#pragma once

#include "core.h"

constexpr int PUZZLE_PIECES = 48;
constexpr int CREATURE_LEVELS = 7;
constexpr int MAX_CREATURES = 3000;

constexpr char const* ANY_LAND = "any_land";

class Puzzle : public fstruct
{
	F_STRUCT(Puzzle) {}

	F_PROPERTY(pieces_pcx_part, fstring) { value = "cas"; } // todo: default puzzle map
	F_PROPERTY(pieces_x, farray<fint, PUZZLE_PIECES>) {}
	F_PROPERTY(pieces_y, farray<fint, PUZZLE_PIECES>) {}
	F_PROPERTY(pieces_order, farray<fint, PUZZLE_PIECES>) {}
};

class CreatureAnimation : public fstruct
{
	F_STRUCT(CreatureAnimation) {}

	F_PROPERTY(time_between_fidgets_multiplier, ffloat) { value = 1; optional = true; }
	F_PROPERTY(walk_animation_time_multiplier, ffloat) { value = 1; optional = true; }
	F_PROPERTY(attack_animation_time_multiplier, ffloat) { value = 1; optional = true; }
	F_PROPERTY(fight_animation_distance_multiplier, ffloat) { value = 1; optional = true; }
	F_PROPERTY(missile_offsets, farray<fvector2, 3>) { optional = true; }
	F_PROPERTY(missile_frame_angles, farray<ffloat, 12>) { optional = true; }
	F_PROPERTY(troop_count_location_offset, fint) { optional = true; }
	F_PROPERTY(attack_climax_frame, fint) { optional = true; }

public:
	int time_between_fidgets() { return int(9000 * time_between_fidgets_multiplier); }
	int walk_animation_time() { return int(500 * walk_animation_time_multiplier); }
	int attack_animation_time() { return int(500 * attack_animation_time_multiplier); }
	int fight_animation_distance() { return int(115 * fight_animation_distance_multiplier); }
};

class Appearance : public fstruct
{
	F_STRUCT(Appearance) {}

	F_PROPERTY(def, fstring) {}
	F_PROPERTY(width, fint) {}
	F_PROPERTY(height, fint) {}
	F_PROPERTY(colors, flist<fvector2>) {}
	F_PROPERTY(shadows, flist<fvector2>) {}
	F_PROPERTY(impassability, flist<fvector2>) {}
	F_PROPERTY(entrances, flist<fvector2>) {}
	F_PROPERTY(terrains, flist<fstring>) { value.add(ANY_LAND); optional = true; }
	F_PROPERTY(is_flat, fbool) { optional = true; }
};

class Game : public fstruct
{
public:
	class Town;
	class Terrain;
	class Resource;
	class Resources;
	class Creature;
	class UnsortedMapObject;

	class Town : public fclass
	{
		F_CLASS(Town) {}

		F_PROPERTY(native_terrain, fref<Terrain>) {}
		F_PROPERTY(rmg_terrain, fref<Terrain>) {}

		F_PROPERTY(mp3, fstring) { value = "Quiet"; }

		F_PROPERTY(name, fstring) { value = "Town"; }
		F_PROPERTY(rmg_name_lowercase, fstring) { value = "town"; }

		F_PROPERTY(town_object_names, flist<fstring>) {}

		F_PROPERTY(puzzle, Puzzle) {}

		F_PROPERTY(map_def_village, fstring) { value = "avcdefault.def"; }
		F_PROPERTY(map_def_castle, fstring) { value = "avcdefault.def"; }
		F_PROPERTY(map_def_capitol, fstring) { value = "avcdefault.def"; }

		F_PROPERTY(appearance, Appearance) {}
		F_PROPERTY(random_dwelling_appearance, Appearance) { optional = true; }
	};

	class Terrain : public fclass
	{
		F_CLASS(Terrain) {}
	};

	class Resource : public fclass
	{
		F_CLASS(Resource) {}
	};

	class Resources : public fautodict<fref<Resource>, fint>
	{
	public:
		Resources() {}
		Resources(Resources const& value) : fautodict(value) {}
	};

	class Creature : public fclass
	{
		F_CLASS(Creature) {}

		F_PROPERTY(is_allowed_in_editor, fbool) {}
		F_PROPERTY(is_allowed_in_random, fbool) {}

		F_PROPERTY(native_town, fref<Town>) {}
		F_PROPERTY(level, fint) { value = -1; } // todo: set default level to 0

		F_PROPERTY(sound_prefix, fstring) { value = "lcrs"; } // todo: default sound
		F_PROPERTY(battle_def, fstring) { value = "clcbow.def"; } // todo: default def
		F_PROPERTY(missle_def, fstring) { optional = true; } // todo: default def

		F_PROPERTY(monster_def, fstring) { optional = true; }
		F_PROPERTY(monster_attack_def, fstring) { value = "avwattak.def:244"; optional = true; } // todo: default def

		F_PROPERTY(interface_def, fstring) { value = "twcrport.def:124"; } // todo: default def
		F_PROPERTY(interface_def_small, fstring) { value = "cprsmall.def:124"; } // todo: default def

		F_PROPERTY(double_wide, fbool) { optional = true; }
		F_PROPERTY(flying, fbool) { optional = true; }
		F_PROPERTY(shooting, fbool) { optional = true; }
		F_PROPERTY(dragon_breath, fbool) { optional = true; }
		F_PROPERTY(is_alive, fbool) { value = true; optional = true; }
		F_PROPERTY(can_attack_walls, fbool) { optional = true; }
		F_PROPERTY(is_siege_weapon, fbool) { optional = true; }
		F_PROPERTY(king_level, fint) { optional = true; }
		F_PROPERTY(mind_immunity, fbool) { optional = true; }
		F_PROPERTY(shooting_ray, fbool) { optional = true; }
		F_PROPERTY(no_melee_penalty, fbool) { optional = true; }
		F_PROPERTY(fire_immunity, fbool) { optional = true; }
		F_PROPERTY(attack_twice, fbool) { optional = true; }
		F_PROPERTY(no_retaliation, fbool) { optional = true; }
		F_PROPERTY(no_morale, fbool) { optional = true; }
		F_PROPERTY(is_undead, fbool) { optional = true; }
		F_PROPERTY(attack_all_around, fbool) { optional = true; }
		F_PROPERTY(fireball_attack, fbool) { optional = true; }
		F_PROPERTY(is_dragon, fbool) { optional = true; }

		F_PROPERTY(name_singular, fstring) { value = "Creature"; }
		F_PROPERTY(name_plural, fstring) { value = "Creatures"; }
		F_PROPERTY(description, fstring) {}

		F_PROPERTY(cost, Resources) { value["gold"] = 100; }
		F_PROPERTY(fight_value, fint) { value = 100; }
		F_PROPERTY(ai_value, fint) { value = 100; }
		F_PROPERTY(growth, fint) { value = 10; }
		F_PROPERTY(horde_growth, fint) { optional = true; } // todo: remove
		F_PROPERTY(hit_points, fint) { value = 10; }
		F_PROPERTY(speed, fint) { value = 5; }
		F_PROPERTY(attack, fint) { value = 5; }
		F_PROPERTY(defence, fint) { value = 5; }
		F_PROPERTY(damage_low, fint) { value = 1; }
		F_PROPERTY(damage_high, fint) { value = 1; }
		F_PROPERTY(num_shots, fint) { optional = true; }
		F_PROPERTY(num_spells, fint) { optional = true; }
		F_PROPERTY(adv_map_low, fint) { value = 10; }
		F_PROPERTY(adv_map_high, fint) { value = 10; }

		F_PROPERTY(animation, CreatureAnimation) { optional = true; }

		F_PROPERTY(skeltrans_immune, fbool) { optional = true; }
		F_PROPERTY(skeltrans_dragon, fbool) { optional = true; }
		F_PROPERTY(skeltrans_knight, fbool) { optional = true; }
	};

	class UnsortedMapObject : public fstruct
	{
		F_STRUCT(UnsortedMapObject) {}

		F_PROPERTY(type, fint) {}
		F_PROPERTY(subtype, fint) {}

		F_PROPERTY(appearance, Appearance) {}
	};

	F_STRUCT(Game) {}

	F_PROPERTY(towns, fstorage<Town>) {}
	F_PROPERTY(terrains, fstorage<Terrain>) {}
	F_PROPERTY(resources, fstorage<Resource>) {}
	F_PROPERTY(creatures, fstorage<Creature>) { value.total_native_slots = MAX_CREATURES; }

	F_PROPERTY(unsorted_map_objects, flist<UnsortedMapObject>) {}

public:
	void initialize();
};

using Town = Game::Town;
using Terrain = Game::Terrain;
using Resource = Game::Resource;
using Resources = Game::Resources;
using Creature = Game::Creature;
using UnsortedMapObject = Game::UnsortedMapObject;
