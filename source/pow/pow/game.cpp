#include "pch.h"
#include "game.h"

void Game::initialize()
{
	towns.fixed_native_slots.add("castle");
	towns.fixed_native_slots.add("rampart");
	towns.fixed_native_slots.add("tower");
	towns.fixed_native_slots.add("inferno");
	towns.fixed_native_slots.add("necropolis");
	towns.fixed_native_slots.add("dungeon");
	towns.fixed_native_slots.add("stronghold");
	towns.fixed_native_slots.add("fortress");
	towns.fixed_native_slots.add("conflux");

	if (exe_version == ExeVersion::hota)
	{
		towns.fixed_native_slots.add("hota_cove");
	}

	terrains.fixed_native_slots.add("dirt");
	terrains.fixed_native_slots.add("sand");
	terrains.fixed_native_slots.add("grass");
	terrains.fixed_native_slots.add("snow");
	terrains.fixed_native_slots.add("swamp");
	terrains.fixed_native_slots.add("rough");
	terrains.fixed_native_slots.add("cave");
	terrains.fixed_native_slots.add("lava");
	terrains.fixed_native_slots.add("water");
	terrains.fixed_native_slots.add("rock");

	if (exe_version == ExeVersion::hota)
	{
		terrains.fixed_native_slots.add("hota_highlands");
		terrains.fixed_native_slots.add("hota_wasteland");
	}

	resources.fixed_native_slots.add("wood");
	resources.fixed_native_slots.add("mercury");
	resources.fixed_native_slots.add("ore");
	resources.fixed_native_slots.add("sulfur");
	resources.fixed_native_slots.add("crystal");
	resources.fixed_native_slots.add("gems");
	resources.fixed_native_slots.add("gold");

	if (exe_version == ExeVersion::wog || exe_version == ExeVersion::era)
	{
		resources.fixed_native_slots.add("wog_mithril");
	}

	creatures.fixed_native_slots.add("creature_pikeman");
	creatures.fixed_native_slots.add("creature_halberdier");
	creatures.fixed_native_slots.add("creature_archer");
	creatures.fixed_native_slots.add("creature_marksman");
	creatures.fixed_native_slots.add("creature_griffin");
	creatures.fixed_native_slots.add("creature_royal_griffin");
	creatures.fixed_native_slots.add("creature_swordsman");
	creatures.fixed_native_slots.add("creature_crusader");
	creatures.fixed_native_slots.add("creature_monk");
	creatures.fixed_native_slots.add("creature_zealot");
	creatures.fixed_native_slots.add("creature_cavalier");
	creatures.fixed_native_slots.add("creature_champion");
	creatures.fixed_native_slots.add("creature_angel");
	creatures.fixed_native_slots.add("creature_archangel");

	creatures.fixed_native_slots.add("creature_centaur");
	creatures.fixed_native_slots.add("creature_centaur_captain");
	creatures.fixed_native_slots.add("creature_dwarf");
	creatures.fixed_native_slots.add("creature_battle_dwarf");
	creatures.fixed_native_slots.add("creature_wood_elf");
	creatures.fixed_native_slots.add("creature_grand_elf");
	creatures.fixed_native_slots.add("creature_pegasus");
	creatures.fixed_native_slots.add("creature_silver_pegasus");
	creatures.fixed_native_slots.add("creature_dendroid_guard");
	creatures.fixed_native_slots.add("creature_dendroid_soldier");
	creatures.fixed_native_slots.add("creature_unicorn");
	creatures.fixed_native_slots.add("creature_war_unicorn");
	creatures.fixed_native_slots.add("creature_green_dragon");
	creatures.fixed_native_slots.add("creature_gold_dragon");

	creatures.fixed_native_slots.add("creature_gremlin");
	creatures.fixed_native_slots.add("creature_master_gremlin");
	creatures.fixed_native_slots.add("creature_stone_gargoyle");
	creatures.fixed_native_slots.add("creature_obsidian_gargoyle");
	creatures.fixed_native_slots.add("creature_stone_golem");
	creatures.fixed_native_slots.add("creature_iron_golem");
	creatures.fixed_native_slots.add("creature_mage");
	creatures.fixed_native_slots.add("creature_arch_mage");
	creatures.fixed_native_slots.add("creature_genie");
	creatures.fixed_native_slots.add("creature_master_genie");
	creatures.fixed_native_slots.add("creature_naga");
	creatures.fixed_native_slots.add("creature_naga_queen");
	creatures.fixed_native_slots.add("creature_giant");
	creatures.fixed_native_slots.add("creature_titan");

	creatures.fixed_native_slots.add("creature_imp");
	creatures.fixed_native_slots.add("creature_familiar");
	creatures.fixed_native_slots.add("creature_gog");
	creatures.fixed_native_slots.add("creature_magog");
	creatures.fixed_native_slots.add("creature_hell_hound");
	creatures.fixed_native_slots.add("creature_cerberus");
	creatures.fixed_native_slots.add("creature_demon");
	creatures.fixed_native_slots.add("creature_horned_demon");
	creatures.fixed_native_slots.add("creature_pit_fiend");
	creatures.fixed_native_slots.add("creature_pit_lord");
	creatures.fixed_native_slots.add("creature_efreeti");
	creatures.fixed_native_slots.add("creature_efreet_sultan");
	creatures.fixed_native_slots.add("creature_devil");
	creatures.fixed_native_slots.add("creature_arch_devil");

	creatures.fixed_native_slots.add("creature_skeleton");
	creatures.fixed_native_slots.add("creature_skeleton_warrior");
	creatures.fixed_native_slots.add("creature_walking_dead");
	creatures.fixed_native_slots.add("creature_zombie");
	creatures.fixed_native_slots.add("creature_wight");
	creatures.fixed_native_slots.add("creature_wraith");
	creatures.fixed_native_slots.add("creature_vampire");
	creatures.fixed_native_slots.add("creature_vampire_lord");
	creatures.fixed_native_slots.add("creature_lich");
	creatures.fixed_native_slots.add("creature_power_lich");
	creatures.fixed_native_slots.add("creature_black_knight");
	creatures.fixed_native_slots.add("creature_dread_knight");
	creatures.fixed_native_slots.add("creature_bone_dragon");
	creatures.fixed_native_slots.add("creature_ghost_dragon");

	creatures.fixed_native_slots.add("creature_troglodyte");
	creatures.fixed_native_slots.add("creature_infernal_troglodyte");
	creatures.fixed_native_slots.add("creature_harpy");
	creatures.fixed_native_slots.add("creature_harpy_hag");
	creatures.fixed_native_slots.add("creature_beholder");
	creatures.fixed_native_slots.add("creature_evil_eye");
	creatures.fixed_native_slots.add("creature_medusa");
	creatures.fixed_native_slots.add("creature_medusa_queen");
	creatures.fixed_native_slots.add("creature_minotaur");
	creatures.fixed_native_slots.add("creature_minotaur_king");
	creatures.fixed_native_slots.add("creature_manticore");
	creatures.fixed_native_slots.add("creature_scorpicore");
	creatures.fixed_native_slots.add("creature_red_dragon");
	creatures.fixed_native_slots.add("creature_black_dragon");

	creatures.fixed_native_slots.add("creature_goblin");
	creatures.fixed_native_slots.add("creature_hobgoblin");
	creatures.fixed_native_slots.add("creature_wolf_rider");
	creatures.fixed_native_slots.add("creature_wolf_raider");
	creatures.fixed_native_slots.add("creature_orc");
	creatures.fixed_native_slots.add("creature_orc_chieftain");
	creatures.fixed_native_slots.add("creature_ogre");
	creatures.fixed_native_slots.add("creature_ogre_mage");
	creatures.fixed_native_slots.add("creature_roc");
	creatures.fixed_native_slots.add("creature_thunderbird");
	creatures.fixed_native_slots.add("creature_cyclops");
	creatures.fixed_native_slots.add("creature_cyclops_king");
	creatures.fixed_native_slots.add("creature_behemoth");
	creatures.fixed_native_slots.add("creature_ancient_behemoth");

	creatures.fixed_native_slots.add("creature_gnoll");
	creatures.fixed_native_slots.add("creature_gnoll_marauder");
	creatures.fixed_native_slots.add("creature_lizardman");
	creatures.fixed_native_slots.add("creature_lizard_warrior");
	creatures.fixed_native_slots.add("creature_gorgon");
	creatures.fixed_native_slots.add("creature_mighty_gorgon");
	creatures.fixed_native_slots.add("creature_serpent_fly");
	creatures.fixed_native_slots.add("creature_dragon_fly");
	creatures.fixed_native_slots.add("creature_basilisk");
	creatures.fixed_native_slots.add("creature_greater_basilisk");
	creatures.fixed_native_slots.add("creature_wyvern");
	creatures.fixed_native_slots.add("creature_wyvern_monarch");
	creatures.fixed_native_slots.add("creature_hydra");
	creatures.fixed_native_slots.add("creature_chaos_hydra");

	creatures.fixed_native_slots.add("creature_air_elemental");
	creatures.fixed_native_slots.add("creature_earth_elemental");
	creatures.fixed_native_slots.add("creature_fire_elemental");
	creatures.fixed_native_slots.add("creature_water_elemental");
	creatures.fixed_native_slots.add("creature_gold_golem");
	creatures.fixed_native_slots.add("creature_diamond_golem");

	creatures.fixed_native_slots.add("creature_pixie");
	creatures.fixed_native_slots.add("creature_sprite");
	creatures.fixed_native_slots.add("creature_psychic_elemental");
	creatures.fixed_native_slots.add("creature_magic_elemental");
	creatures.fixed_native_slots.add("");
	creatures.fixed_native_slots.add("creature_ice_elemental");
	creatures.fixed_native_slots.add("");
	creatures.fixed_native_slots.add("creature_magma_elemental");
	creatures.fixed_native_slots.add("");
	creatures.fixed_native_slots.add("creature_storm_elemental");
	creatures.fixed_native_slots.add("");
	creatures.fixed_native_slots.add("creature_energy_elemental");
	creatures.fixed_native_slots.add("creature_firebird");
	creatures.fixed_native_slots.add("creature_phoenix");

	creatures.fixed_native_slots.add("creature_azure_dragon");
	creatures.fixed_native_slots.add("creature_crystal_dragon");
	creatures.fixed_native_slots.add("creature_faerie_dragon");
	creatures.fixed_native_slots.add("creature_rust_dragon");
	creatures.fixed_native_slots.add("creature_enchanter");
	creatures.fixed_native_slots.add("creature_sharpshooter");
	creatures.fixed_native_slots.add("creature_halfling");
	creatures.fixed_native_slots.add("creature_peasant");
	creatures.fixed_native_slots.add("creature_boar");
	creatures.fixed_native_slots.add("creature_mummy");
	creatures.fixed_native_slots.add("creature_nomad");
	creatures.fixed_native_slots.add("creature_rogue");
	creatures.fixed_native_slots.add("creature_troll");

	creatures.fixed_native_slots.add("creature_catapult");
	creatures.fixed_native_slots.add("creature_ballista");
	creatures.fixed_native_slots.add("creature_first_aid_tent");
	creatures.fixed_native_slots.add("creature_ammo_cart");
	creatures.fixed_native_slots.add("creature_arrow_tower");

	if (exe_version == ExeVersion::hota)
	{
		creatures.fixed_native_slots.add("hota_creature_cannon");

		creatures.fixed_native_slots.add("hota_creature_sea_dog");
		creatures.fixed_native_slots.add("");
		creatures.fixed_native_slots.add("hota_creature_nymph");
		creatures.fixed_native_slots.add("hota_creature_oceanid");
		creatures.fixed_native_slots.add("hota_creature_crew_mate");
		creatures.fixed_native_slots.add("hota_creature_seaman");
		creatures.fixed_native_slots.add("hota_creature_pirate");
		creatures.fixed_native_slots.add("hota_creature_corsair");
		creatures.fixed_native_slots.add("hota_creature_stormbird");
		creatures.fixed_native_slots.add("hota_creature_assyd");
		creatures.fixed_native_slots.add("hota_creature_sea_witch");
		creatures.fixed_native_slots.add("hota_creature_sorceress");
		creatures.fixed_native_slots.add("hota_creature_nix");
		creatures.fixed_native_slots.add("hota_creature_nix_warrior");
		creatures.fixed_native_slots.add("hota_creature_sea_serpent");
		creatures.fixed_native_slots.add("hota_creature_haspid");

		creatures.fixed_native_slots.add("hota_creature_satyr");
		creatures.fixed_native_slots.add("hota_creature_fangarm");
		creatures.fixed_native_slots.add("hota_creature_leprechaun");
		creatures.fixed_native_slots.add("hota_creature_steel_golem");
	}

	if (exe_version == ExeVersion::wog || exe_version == ExeVersion::era)
	{
		creatures.fixed_native_slots.add("wog_creature_supreme_archangel");
		creatures.fixed_native_slots.add("wog_creature_diamond_dragon");
		creatures.fixed_native_slots.add("wog_creature_lord_of_thunder");
		creatures.fixed_native_slots.add("wog_creature_antichrist");
		creatures.fixed_native_slots.add("wog_creature_blood_dragon");
		creatures.fixed_native_slots.add("wog_creature_darkness_dragon");
		creatures.fixed_native_slots.add("wog_creature_ghost_behemoth");
		creatures.fixed_native_slots.add("wog_creature_hell_hydra");
		creatures.fixed_native_slots.add("wog_creature_sacred_phoenix");

		creatures.fixed_native_slots.add("wog_creature_ghost");

		creatures.fixed_native_slots.add("wog_creature_emissary_of_war");
		creatures.fixed_native_slots.add("wog_creature_emissary_of_peace");
		creatures.fixed_native_slots.add("wog_creature_emissary_of_mana");
		creatures.fixed_native_slots.add("wog_creature_emissary_of_lore");
		creatures.fixed_native_slots.add("wog_creature_fire_messenger");
		creatures.fixed_native_slots.add("wog_creature_earth_messenger");
		creatures.fixed_native_slots.add("wog_creature_air_messenger");
		creatures.fixed_native_slots.add("wog_creature_water_messenger");

		creatures.fixed_native_slots.add("wog_creature_gorynych");
		creatures.fixed_native_slots.add("wog_creature_war_zealot");
		creatures.fixed_native_slots.add("wog_creature_arctic_sharpshooter");
		creatures.fixed_native_slots.add("wog_creature_lava_sharpshooter");
		creatures.fixed_native_slots.add("wog_creature_nightmare");
		creatures.fixed_native_slots.add("wog_creature_santa_gremlin");

		creatures.fixed_native_slots.add("wog_creature_npc00");
		creatures.fixed_native_slots.add("wog_creature_npc01");
		creatures.fixed_native_slots.add("wog_creature_npc02");
		creatures.fixed_native_slots.add("wog_creature_npc03");
		creatures.fixed_native_slots.add("wog_creature_npc04");
		creatures.fixed_native_slots.add("wog_creature_npc05");
		creatures.fixed_native_slots.add("wog_creature_npc06");
		creatures.fixed_native_slots.add("wog_creature_npc07");
		creatures.fixed_native_slots.add("wog_creature_npc08");
		creatures.fixed_native_slots.add("wog_creature_npc10");
		creatures.fixed_native_slots.add("wog_creature_npc11");
		creatures.fixed_native_slots.add("wog_creature_npc12");
		creatures.fixed_native_slots.add("wog_creature_npc13");
		creatures.fixed_native_slots.add("wog_creature_npc14");
		creatures.fixed_native_slots.add("wog_creature_npc15");
		creatures.fixed_native_slots.add("wog_creature_npc16");
		creatures.fixed_native_slots.add("wog_creature_npc17");
		creatures.fixed_native_slots.add("wog_creature_npc18");

		creatures.fixed_native_slots.add("wog_creature_sylvan_centaur");
		creatures.fixed_native_slots.add("wog_creature_sorceress");
		creatures.fixed_native_slots.add("wog_creature_werewolf");
		creatures.fixed_native_slots.add("wog_creature_hell_steed");
		creatures.fixed_native_slots.add("wog_creature_dracolich");
	}
}
