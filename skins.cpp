#include "includes.h"
#include <codecvt>

/*
░██████╗██╗░░░██╗██████╗░██████╗░███████╗███╗░░░███╗░█████╗░░█████╗░██╗░░░██╗  ░█████╗░░██████╗██╗░██████╗░░█████╗░
██╔════╝██║░░░██║██╔══██╗██╔══██╗██╔════╝████╗░████║██╔══██╗██╔══██╗╚██╗░██╔╝  ██╔══██╗██╔════╝╚═╝██╔════╝░██╔══██╗
╚█████╗░██║░░░██║██████╔╝██████╔╝█████╗░░██╔████╔██║███████║██║░░╚═╝░╚████╔╝░  ██║░░╚═╝╚█████╗░░░░██║░░██╗░██║░░██║
░╚═══██╗██║░░░██║██╔═══╝░██╔══██╗██╔══╝░░██║╚██╔╝██║██╔══██║██║░░██╗░░╚██╔╝░░  ██║░░██╗░╚═══██╗░░░██║░░╚██╗██║░░██║
██████╔╝╚██████╔╝██║░░░░░██║░░██║███████╗██║░╚═╝░██║██║░░██║╚█████╔╝░░░██║░░░  ╚█████╔╝██████╔╝██╗╚██████╔╝╚█████╔╝
╚═════╝░░╚═════╝░╚═╝░░░░░╚═╝░░╚═╝╚══════╝╚═╝░░░░░╚═╝╚═╝░░╚═╝░╚════╝░░░░╚═╝░░░  ░╚════╝░╚═════╝░╚═╝░╚═════╝░░╚════╝░
*/

// Corrections
// TODO additional info (quality? stat track?)
// The bug with hands and gloves on the ground >_< - force fullupdate on local player's death as temp fix (event player death)...

Skins g_skins{};

void Skins::load() {
	// update model indexes on mapload.
	m_knife_data[knives_t::BAYONET].setup(KNIFE_BAYONET,
		XOR("models/weapons/v_knife_bayonet.mdl"),
		XOR("models/weapons/w_knife_bayonet.mdl"),
		XOR("bayonet"));

	m_knife_data[knives_t::BOWIE].setup(KNIFE_BOWIE,
		XOR("models/weapons/v_knife_survival_bowie.mdl"),
		XOR("models/weapons/w_knife_survival_bowie.mdl"),
		XOR("knife_survival_bowie"));


	m_knife_data[knives_t::BUTTERFLY].setup(KNIFE_BUTTERFLY,
		XOR("models/weapons/v_knife_butterfly.mdl"),
		XOR("models/weapons/w_knife_butterfly.mdl"),
		XOR("knife_butterfly"));

	m_knife_data[knives_t::FALCHION].setup(KNIFE_FALCHION,
		XOR("models/weapons/v_knife_falchion_advanced.mdl"),
		XOR("models/weapons/w_knife_falchion_advanced.mdl"),
		XOR("knife_falchion"));

	m_knife_data[knives_t::FLIP].setup(KNIFE_FLIP,
		XOR("models/weapons/v_knife_flip.mdl"),
		XOR("models/weapons/w_knife_flip.mdl"),
		XOR("knife_flip"));

	m_knife_data[knives_t::GUT].setup(KNIFE_GUT,
		XOR("models/weapons/v_knife_gut.mdl"),
		XOR("models/weapons/w_knife_gut.mdl"),
		XOR("knife_gut"));

	m_knife_data[knives_t::HUNTSMAN].setup(KNIFE_HUNTSMAN,
		XOR("models/weapons/v_knife_tactical.mdl"),
		XOR("models/weapons/w_knife_tactical.mdl"),
		XOR("knife_tactical"));

	m_knife_data[knives_t::KARAMBIT].setup(KNIFE_KARAMBIT,
		XOR("models/weapons/v_knife_karam.mdl"),
		XOR("models/weapons/w_knife_karam.mdl"),
		XOR("knife_karambit"));

	m_knife_data[knives_t::M9].setup(KNIFE_M9_BAYONET,
		XOR("models/weapons/v_knife_m9_bay.mdl"),
		XOR("models/weapons/w_knife_m9_bay.mdl"),
		XOR("knife_m9_bayonet"));

	m_knife_data[knives_t::DAGGER].setup(KNIFE_SHADOW_DAGGERS,
		XOR("models/weapons/v_knife_push.mdl"),
		XOR("models/weapons/w_knife_push.mdl"),
		XOR("knife_push"));

	// update glove model indexes on mapload.
	m_glove_data[gloves_t::BLOODHOUND].setup(5027,
		XOR("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"),
		XOR("models/weapons/w_models/arms/glove_bloodhound/w_glove_bloodhound.mdl"));

	m_glove_data[gloves_t::SPORTY].setup(5030,
		XOR("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"),
		XOR("models/weapons/w_models/arms/glove_sporty/w_glove_sporty.mdl"));

	m_glove_data[gloves_t::DRIVER].setup(5031,
		XOR("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"),
		XOR("models/weapons/w_models/arms/glove_slick/w_glove_slick.mdl"));

	m_glove_data[gloves_t::HANDWRAP].setup(5032,
		XOR("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"),
		XOR("models/weapons/w_models/arms/glove_handwrap_leathery/w_glove_handwrap_leathery.mdl"));

	m_glove_data[gloves_t::MOTO].setup(5033,
		XOR("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"),
		XOR("models/weapons/w_models/arms/glove_motorcycle/w_glove_motorcycle.mdl"));

	m_glove_data[gloves_t::SPECIALIST].setup(5034,
		XOR("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"),
		XOR("models/weapons/w_models/arms/glove_specialist/w_glove_specialist.mdl"));

	m_update_time = 0.f;
}

void Skins::think() {
	std::vector< Weapon* > weapons{};

	if (!g_config.b["skins_enable"])
		return;

	if (!g_csgo.m_engine->IsInGame())
		return;

	if (!g_cl.m_local || g_cl.m_local->m_iHealth() < 1)
		return;

	player_info_t info;
	if (!g_csgo.m_engine->GetPlayerInfo(g_cl.m_local->index(), &info))
		return;

	// store knife index.
	KnifeData* knife = &m_knife_data[g_config.i["skins_knife"]];

	for (int i{ 1 }; i <= g_csgo.m_entlist->GetHighestEntityIndex(); ++i) {
		Entity* ent = g_csgo.m_entlist->GetClientEntity(i);
		if (!ent)
			continue;

		// run knifechanger.
		if (g_config.i["skins_knife"] > 0 && ent->is(HASH("CPredictedViewModel"))) {
			// get weapon entity from predicted viewmodel.
			Weapon* weapon = ent->as< Weapon* >()->GetWeapon();

			// no need to be here anymore.
			if (!weapon || !weapon->IsKnife())
				continue;

			// this weapon does not belong to us, we are done here.
			// should barely happen unless you pick up someones knife.
			// possible on servers with 'mp_drop_knife_enable 1'.
			if (info.m_xuid_low != weapon->m_OriginalOwnerXuidLow() || info.m_xuid_high != weapon->m_OriginalOwnerXuidHigh())
				continue;

			// get and validate world model handle from the weapon
			Weapon* weapon_world_model = weapon->GetWeaponWorldModel();
			if (!weapon_world_model)
				continue;

			// set model index of the predicted viewmodel.
			ent->m_nModelIndex() = knife->m_model_index;

			// set the world model index.
			// do this to have knifechanger in third person. verry p.
			weapon_world_model->m_nModelIndex() = knife->m_world_model_index;

			// correct m_nSequence and m_flCycle.
			UpdateAnimations(ent);
		}

		else if (ent->IsBaseCombatWeapon()) {
			// cast to weapon class.
			Weapon* weapon = ent->as< Weapon* >();

			// this is not our gun.
			if (info.m_xuid_low != weapon->m_OriginalOwnerXuidLow() || info.m_xuid_high != weapon->m_OriginalOwnerXuidHigh())
				continue;

			// we found a weapon that we own.
			weapons.push_back(weapon);

			if (weapon->IsKnife()) {
				// if this weapon is a knife, set some additional stuff.
				if (g_config.i["skins_knife"] > 0) {
					// set the item id, this is for the hud.
					weapon->m_iItemDefinitionIndex() = knife->m_id;

					// not needed. but everyone does this, try without.
					weapon->m_nModelIndex() = knife->m_model_index;

					// not needed. but everyone does this, try without.
					weapon->m_iViewModelIndex() = knife->m_model_index;

					// not needed. but everyone does this, try without.
					weapon->m_iWorldModelIndex() = knife->m_world_model_index;

					// set the little star thing.
					weapon->m_iEntityQuality() = 3;
					//weapon->m_nFallbackPaintKit() = g_menu.main.skins.knife_id.get();	// hmm
				}

				else if (g_config.i["skins_knife"] == 0) {
					// fix definition.
					if (g_cl.m_local->m_iTeamNum() == TEAM_TERRORISTS)
						weapon->m_iItemDefinitionIndex() = KNIFE_T;

					else if (g_cl.m_local->m_iTeamNum() == TEAM_COUNTERTERRORISTS)
						weapon->m_iItemDefinitionIndex() = KNIFE_CT;

					// reset.
					weapon->m_iEntityQuality() = 0;
					weapon->m_nFallbackPaintKit() = -1;
					weapon->m_nFallbackStatTrak() = -1;
				}
			}

			switch (weapon->m_iItemDefinitionIndex()) {
			case DEAGLE:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_1_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_1_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_1_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_1_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case ELITE:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_2_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_2_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_2_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_2_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case FIVESEVEN:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_3_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_3_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_3_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_3_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case GLOCK:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_4_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_4_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_4_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_4_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case AK47:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_7_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_7_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_7_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_7_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case AUG:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_8_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_8_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_8_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_8_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case AWP:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_9_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_9_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_9_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_9_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case FAMAS:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_10_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_10_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_10_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_10_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case G3SG1:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_11_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_11_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_11_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_11_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case GALIL:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_13_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_13_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_13_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_13_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case M249:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_14_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_14_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_14_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_14_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case M4A4:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_16_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_16_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_16_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_16_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case MAC10:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_17_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_17_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_17_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_17_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case P90:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_19_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_19_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_19_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_19_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case UMP45:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_24_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_24_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_24_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_24_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case XM1014:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_25_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_25_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_25_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_25_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case BIZON:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_26_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_26_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_26_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_26_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case MAG7:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_27_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_27_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_27_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_27_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case NEGEV:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_28_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_28_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_28_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_28_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case SAWEDOFF:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_29_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_29_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_29_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_29_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case TEC9:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_30_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_30_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_30_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_30_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case P2000:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_32_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_32_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_32_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_32_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case MP7:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_33_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_33_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_33_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_33_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case MP9:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_34_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_34_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_34_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_34_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case NOVA:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_35_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_35_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_35_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_35_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case P250:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_36_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_36_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_36_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_36_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case SCAR20:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_38_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_38_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_38_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_38_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case SG553:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_39_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_39_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_39_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_39_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case SSG08:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_40_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_40_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_40_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_40_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case M4A1S:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_60_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_60_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_60_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_60_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case USPS:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_61_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_61_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_61_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_61_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case CZ75A:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_63_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_63_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_63_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_63_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case REVOLVER:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_64_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_64_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_640_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_64_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_BAYONET:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_500_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_500_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_500_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_500_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_FLIP:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_505_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_505_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_505_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_505_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_GUT:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_506_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_506_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_506_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_506_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_KARAMBIT:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_507_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_507_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_507_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_507_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_M9_BAYONET:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_508_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_508_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_508_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_508_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_HUNTSMAN:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_509_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_509_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_509_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_509_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_FALCHION:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_512_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_512weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_512_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_512_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_BOWIE:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_514_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_514_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_514_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_514_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_BUTTERFLY:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_515_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_515_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_515_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_515_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;
			case KNIFE_SHADOW_DAGGERS:
				weapon->m_nFallbackPaintKit() = g_config.i["skins_516_weapon"];
				weapon->m_nFallbackStatTrak() = g_config.i["skins_516_weapon_stat"];
				weapon->m_nFallbackSeed() = g_config.i["skins_516_weapon_seed"];
				weapon->m_flFallbackWear() = ((100.f - g_config.i["skins_516_weapon_wear"]) / 100.f) + FLT_EPSILON;
				break;

			default:
				break;
			}

			// fix stattrak ownership.
			weapon->m_iAccountID() = info.m_xuid_low;

			// fix stattrak in hud.
			if (weapon->m_nFallbackStatTrak() >= 0)
				weapon->m_iEntityQuality() = 9;

			// force use fallback values.
			weapon->m_iItemIDHigh() = -1;
		}
	}

	if (g_config.i["skins_glove"] > 0) {
		CBaseHandle* wearables = g_cl.m_local->m_hMyWearables();

		// get pointer to entity from wearable handle 0.
		// glove is at wearable 0, valve will likely add more wearables like hats and boots. thats why.
		Weapon* glove = g_csgo.m_entlist->GetClientEntityFromHandle< Weapon* >(wearables[0]);

		// there is no glove entity yet.
		if (!glove) {
			// attempt to get our old glove.
			Weapon* old = g_csgo.m_entlist->GetClientEntityFromHandle< Weapon* >(m_glove_handle);

			// this glove is still valid.
			if (old) {

				// write back handle to wearables.
				wearables[0] = m_glove_handle;

				// set glove pointer.
				glove = old;
			}
		}

		// if we at this point still not have a glove entity.
		// we should create one.
		if (!glove) {
			ClientClass* list{ g_csgo.m_client->GetAllClasses() };

			// iterate list.
			for (; list != nullptr; list = list->m_pNext) {

				// break if we found wearable
				if (list->m_ClassID == g_netvars.GetClientID(HASH("CEconWearable")))
					break;
			}

			// create an ent index and serial no for the new glove entity.
			int index = g_csgo.m_entlist->GetHighestEntityIndex() + 1;
			int serial = g_csgo.RandomInt(0xA00, 0xFFF);

			// call ctor on CEconWearable entity.
			Address networkable = list->m_pCreate(index, serial);

			// get entity ptr via index.
			glove = g_csgo.m_entlist->GetClientEntity< Weapon* >(index);

			static Address offset = g_netvars.get(HASH("DT_EconEntity"), HASH("m_iItemIDLow"));

			// m_bInitialized?
			networkable.add(offset.add(0x8)).set< bool >(true);

			// no idea what this is.
			networkable.add(offset.sub(0xC)).set< bool >(true);

			// set the wearable handle.
			wearables[0] = index | (serial << 16);

			// use this for later on.
			m_glove_handle = wearables[0];
		}

		// store glove data.
		GloveData* data = &m_glove_data[g_config.i["skins_glove"]];

		// set default data,
		glove->m_nFallbackSeed() = 0;
		glove->m_nFallbackStatTrak() = -1;
		glove->m_iItemIDHigh() = -1;
		glove->m_iEntityQuality() = 4;
		glove->m_iAccountID() = info.m_xuid_low;

		// set custom data.
		glove->m_nFallbackPaintKit() = GetGlovePainkit();
		glove->m_iItemDefinitionIndex() = data->m_id;
		glove->SetGloveModelIndex(data->m_model_index);

		// update the glove.
		glove->PreDataUpdate(DATA_UPDATE_CREATED);
	}

	// only force update every 1s.
	if (m_update && g_csgo.m_globals->m_curtime >= m_update_time) {
		for (auto& w : weapons)
			UpdateItem(w);

		m_update = false;
		m_update_time = g_csgo.m_globals->m_curtime + 1.f;
	}
}

int Skins::GetGlovePainkit() {
	std::vector<int> glove_skins = {};
	switch (g_config.i["skins_glove"]) {
	case 1: glove_skins = { 10057, 10058,10059, 10060 }; break;
	case 2: glove_skins = { 10048, 10047, 10045, 10046 }; break;
	case 3: glove_skins = { 10043, 10044, 10041, 10042 }; break;
	case 4: glove_skins = { 10053, 10054, 10055, 10056 }; break;
	case 5: glove_skins = { 10050, 10051, 10052, 10049 }; break;
	case 6: glove_skins = { 10061, 10062, 10063, 10064 }; break;
	default:
		break;
	}

	return glove_skins[g_config.i["skins_glove_skin"]];
}

void Skins::UpdateGloves() {
	/*static auto old = g_config.i["skins_glove_skin"];
	if (old != g_config.i["skins_glove_skin"])
	{
		g_csgo.m_cl->m_delta_tick = -1;
		old = g_config.i["skins_glove_skin"];
	}*/
}

void Skins::UpdateItem(Weapon* item) {
	if (!item || !item->IsBaseCombatWeapon())
		return;

	if (g_csgo.m_cl->m_delta_tick == -1)
		return;

	item->m_bCustomMaterialInitialized() = item->m_nFallbackPaintKit() <= 0;

	item->m_CustomMaterials().RemoveAll();
	item->m_CustomMaterials2().RemoveAll();

	size_t count = item->m_VisualsDataProcessors().Count();
	for (size_t i{}; i < count; ++i) {
		auto& elem = item->m_VisualsDataProcessors()[i];
		if (elem) {
			elem->unreference();
			elem = nullptr;
		}
	}

	item->m_VisualsDataProcessors().RemoveAll();

	item->PostDataUpdate(DATA_UPDATE_CREATED);
	item->OnDataChanged(DATA_UPDATE_CREATED);

	/*CHudElement* SFWeaponSelection = g_csgo.m_hud->FindElement(HASH("SFWeaponSelection"));
	g_csgo.ShowAndUpdateSelection(SFWeaponSelection, 0, item, false);*/
}

void Skins::UpdateAnimations(Entity* ent) {
	int knife = g_config.i["skins_knife"];

	int seq = ent->m_nSequence();

	// check if this knife needs extra fixing.
	if (knife == knives_t::BUTTERFLY || knife == knives_t::FALCHION || knife == knives_t::DAGGER || knife == knives_t::BOWIE) {

		// fix the idle sequences.
		if (seq == sequence_default_idle1 || seq == sequence_default_idle2) {
			// set the animation to be completed.
			ent->m_flCycle() = 0.999f;

			// cycle change, re-render.
			ent->InvalidatePhysicsRecursive(ANIMATION_CHANGED);
		}
	}

	// fix sequences.
	if (m_last_seq != seq) {
		if (knife == knives_t::BUTTERFLY) {
			switch (seq) {
			case sequence_default_draw:
				seq = g_csgo.RandomInt(sequence_butterfly_draw, sequence_butterfly_draw2);
				break;

			case sequence_default_lookat01:
				seq = g_csgo.RandomInt(sequence_butterfly_lookat01, sequence_butterfly_lookat03);
				break;

			default:
				seq++;
				break;
			}
		}

		else if (knife == knives_t::FALCHION) {
			switch (seq) {
			case sequence_default_draw:
			case sequence_default_idle1:
				break;

			case sequence_default_idle2:
				seq = sequence_falchion_idle1;
				break;

			case sequence_default_heavy_miss1:
				seq = g_csgo.RandomInt(sequence_falchion_heavy_miss1, sequence_falchion_heavy_miss1_noflip);
				break;

			case sequence_default_lookat01:
				seq = g_csgo.RandomInt(sequence_falchion_lookat01, sequence_falchion_lookat02);
				break;
			}
		}

		else if (knife == knives_t::DAGGER) {
			switch (seq) {
			case sequence_default_idle2:
				seq = sequence_push_idle1;
				break;

			case sequence_default_heavy_hit1:
			case sequence_default_heavy_backstab:
			case sequence_default_lookat01:
				seq += 3;
				break;

			case sequence_default_heavy_miss1:
				seq = sequence_push_heavy_miss2;
				break;
			}

		}

		else if (knife == knives_t::BOWIE) {
			if (seq > sequence_default_idle1)
				seq--;
		}

		m_last_seq = seq;
	}

	// write back fixed sequence.
	ent->m_nSequence() = seq;
}


class CCStrike15ItemSchema;
class CCStrike15ItemSystem;

void Skins::init()
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	const auto sig_address = pattern::find(g_csgo.m_client_dll, XOR("E8 ? ? ? ? FF 76 0C 8D 48 04 E8"));
	const auto item_system_offset = *sig_address.add(0x1).as< std::int32_t*>();
	const auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address.add(0x5) + item_system_offset);
	const auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(std::uintptr_t(item_system_fn()) + sizeof(void*));

	const auto get_paint_kit_definition_offset = *sig_address.add(12).as< std::int32_t*>();
	const auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address.add(16) + get_paint_kit_definition_offset);
	const auto start_element_offset = *reinterpret_cast<std::intptr_t*>(std::uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
	const auto head_offset = start_element_offset - 12;
	const auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(std::uintptr_t(item_schema) + head_offset);

	for (auto i = 0; i <= map_head->last_element; ++i) {
		const auto paint_kit = map_head->memory[i].value;

		if (paint_kit->id == 9001)
			continue;

		const auto wide_name = g_csgo.m_localize->Find(paint_kit->item_name.buffer + 1);
		const auto name = converter.to_bytes(wide_name);

		if (paint_kit->id < 10000)
			m_skins.push_back({ paint_kit->id, name });
		else
			m_gloves.push_back({ paint_kit->id, name });
	}

	std::sort(m_skins.begin(), m_skins.end());
	std::sort(m_gloves.begin(), m_gloves.end());

	// copy array contents
	for (auto i = 0; i < m_skins.size(); ++i) {
		m_paint_kits.emplace_back(m_skins[i].name);
	}

	for (auto i = 0; i < m_gloves.size(); ++i) {
		m_glove_kits.emplace_back(m_gloves[i].name);
	}
}