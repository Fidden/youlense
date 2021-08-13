#include "cmenu.hpp"
#include "bg_texture.hpp"
#include "fonts.hpp"
#include <shellapi.h>

void Menu::initialize(IDirect3DDevice9* pDevice) {
	if (this->m_bInitialized)
		return;

	ui::CreateContext();
	auto io = ui::GetIO();
	auto style = &ui::GetStyle();

	style->WindowRounding = 0.f;
	style->AntiAliasedLines = true;
	style->AntiAliasedFill = true;
	style->ScrollbarRounding = 0.f;
	style->ScrollbarSize = 6.f;
	style->WindowPadding = ImVec2(0, 0);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(45 / 255.f, 45 / 255.f, 45 / 255.f, 1.f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(65 / 255.f, 65 / 255.f, 65 / 255.f, 1.f);

	this->m_bIsOpened = true;

	IDirect3DSwapChain9* pChain = nullptr;
	D3DPRESENT_PARAMETERS pp = {};
	D3DDEVICE_CREATION_PARAMETERS param = {};
	pDevice->GetCreationParameters(&param);
	pDevice->GetSwapChain(0, &pChain);

	if (pChain)
		pChain->GetPresentParameters(&pp);

	ImGui_ImplWin32_Init(param.hFocusWindow);
	ImGui_ImplDX9_Init(pDevice);

	D3DXCreateTextureFromFileInMemoryEx(
		pDevice, texture, sizeof(texture), 4096, 4096, D3DX_DEFAULT, NULL,
		pp.BackBufferFormat, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, NULL, NULL, NULL, &this->m_pTexture);

	ImFontConfig cfg;
	io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdana.ttf", 12.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromFileTTF("C:/windows/fonts/verdanab.ttf", 12.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontFromMemoryTTF(keybinds_font, 25600, 10.f, &cfg, io.Fonts->GetGlyphRangesCyrillic());
	io.Fonts->AddFontDefault();

	ImGuiFreeType::BuildFontAtlas(io.Fonts, 0x00);

	this->m_bInitialized = true;
}

void Menu::draw_begin() {
	if (!this->m_bInitialized)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ui::NewFrame();
}

void dmt(std::string key) {
	if (g_config.b[XOR("lua_devmode")] && ui::IsItemHovered()) {
		ui::BeginTooltip();
		ui::Text(key.c_str());
		ui::EndTooltip();
	}
}

void draw_lua_items(std::string tab, std::string container) {
	for (auto i : g_lua.menu_items[tab][container]) {
		if (!i.is_visible)
			continue;

		auto type = i.type;
		switch (type)
		{
		case MENUITEM_CHECKBOX:
			if (ui::Checkbox(i.label.c_str(), &g_config.b[i.key])) {
				if (i.callback != sol::nil)
					i.callback(g_config.b[i.key]);
			}

			dmt(i.key);
			break;
		case MENUITEM_SLIDERINT:
			if (ui::SliderInt(i.label.c_str(), &g_config.i[i.key], i.i_min, i.i_max, i.format.c_str())) {
				if (i.callback != sol::nil)
					i.callback(g_config.i[i.key]);
			}

			dmt(i.key);
			break;
		case MENUITEM_SLIDERFLOAT:
			if (ui::SliderFloat(i.label.c_str(), &g_config.f[i.key], i.f_min, i.f_max, i.format.c_str())) {
				if (i.callback != sol::nil)
					i.callback(g_config.f[i.key]);
			}

			dmt(i.key);
			break;
		case MENUITEM_KEYBIND:
			if (ui::Keybind(i.label.c_str(), &g_config.i[i.key], i.allow_style_change ? &g_config.i[i.key + XOR("style")] : NULL)) {
				if (i.callback != sol::nil)
					i.callback(g_config.i[i.key], g_config.i[i.key + XOR("style")]);
			}

			dmt(i.key + (i.allow_style_change ? " | " + i.key + XOR("style") : XOR("") ));
			break;
		case MENUITEM_TEXT:
			ui::Text(i.label.c_str());
			break;
		case MENUITEM_SINGLESELECT:
			if (ui::SingleSelect(i.label.c_str(), &g_config.i[i.key], i.items)) {
				if (i.callback != sol::nil)
					i.callback(g_config.i[i.key]);
			}

			dmt(i.key);
			break;
		case MENUITEM_MULTISELECT:
			if (ui::MultiSelect(i.label.c_str(), &g_config.m[i.key], i.items)) {
				if (i.callback != sol::nil)
					i.callback(g_config.m[i.key]);
			}

			dmt(i.key);
			break;
		case MENUITEM_COLORPICKER:
			if (ui::ColorEdit4(i.label.c_str(), g_config.c[i.key])) {
				if (i.callback != sol::nil)
					i.callback(g_config.c[i.key][0] * 255, g_config.c[i.key][1] * 255, g_config.c[i.key][2] * 255, g_config.c[i.key][3] * 255);
			}

			dmt(i.key);
			break;
		case MENUITEM_BUTTON:
			if (ui::Button(i.label.c_str())) {
				if (i.callback != sol::nil)
					i.callback();
			}
			break;
		default:
			break;
		}
	}
}

void Menu::draw() {
	if (!this->m_bIsOpened && ui::GetStyle().Alpha > 0.f) {
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha - fc / 255.f, 0.f, 1.f);
	}

	if (this->m_bIsOpened && ui::GetStyle().Alpha < 1.f) {
		float fc = 255.f / 0.2f * ui::GetIO().DeltaTime;
		ui::GetStyle().Alpha = std::clamp(ui::GetStyle().Alpha + fc / 255.f, 0.f, 1.f);
	}

	if (!this->m_bIsOpened && ui::GetStyle().Alpha == 0.f)
		return;

	ui::SetNextWindowSizeConstraints(ImVec2(750, 450), ImVec2(4096, 4096));
	ui::Begin("youlense", 0, ImGuiWindowFlags_NoTitleBar);

	ui::TabButton("Ragebot", &this->m_nCurrentTab, 0, 6);
	ui::TabButton("Anti-aims", &this->m_nCurrentTab, 1, 6);
	ui::TabButton("Visuals", &this->m_nCurrentTab, 2, 6);
	ui::TabButton("Misc", &this->m_nCurrentTab, 3, 6);
	ui::TabButton("Skins", &this->m_nCurrentTab, 4, 6);
	ui::TabButton("Players", &this->m_nCurrentTab, 5, 6);

	static auto calculateChildWindowPosition = [](int num) -> ImVec2 {
		return ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 3 - 31) * num + 20 * num, ui::GetWindowPos().y + 52);
	};

	static auto calculateChildWindowPositionDouble = [](int num) -> ImVec2 {
		return ImVec2(ui::GetWindowPos().x + 26 + (ui::GetWindowSize().x / 2 - 36) * num + 20 * num, ui::GetWindowPos().y + 52);
	};

	auto child_size = ImVec2(ui::GetWindowSize().x / 3 - 31, ui::GetWindowSize().y - 80);
	auto child_size_d = ImVec2(ui::GetWindowSize().x / 2 - 36, ui::GetWindowSize().y - 80);
	auto cfg = g_config;

	if (this->m_nCurrentTab == 0) {
		ui::SetNextWindowPos(calculateChildWindowPosition(0));
		ui::BeginChild("Aimbot", child_size);
		{
			ui::Checkbox("Enable", &g_config.b[XOR("aimbot_enable")]); dmt("aimbot_enable");
			ui::SingleSelect("Target selection", &g_config.i[XOR("aimbot_target_selection")], { "Distance", "Crosshair", "Damage", "Lowest hp", "Least lag", "Height" }); dmt("aimbot_target_selection");
			ui::Checkbox("Silent aim", &g_config.b[XOR("aimbot_silent")]);
			ui::Checkbox("Penetrate walls", &g_config.b[XOR("aimbot_penetration")]);
			
			ui::Checkbox("Force baim", &g_config.b[XOR("rage_forcebaim")]);
			ui::Keybind("##rage_forcebaim_key", &g_config.i[XOR("rage_forcebaim_key")], &g_config.i[XOR("rage_forcebaim_key_style")]);
			ui::Checkbox("Knife-bot", &g_config.b[XOR("aimbot_knife_bot")]);
			ui::Checkbox("Zeus-bot", &g_config.b[XOR("aimbot_zeus_bot")]);
			draw_lua_items("ragebot", "aimbot");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(1));
		ui::BeginChild("Target point selection", child_size);
		{
			ui::MultiSelect("Target Hitbox", &g_config.m[XOR("aimbot_target_hitbox")], { "Head", "Chest", "Stomach", "Arms", "Legs", "Foot" });

			ui::MultiSelect("Multi-point", &g_config.m[XOR("aimbot_target_multipoint")], { "Head", "Chest", "Body", "Legs" });
			if (g_config.m[XOR("aimbot_target_multipoint")][0] || g_config.m[XOR("aimbot_target_multipoint")][3])
				ui::SliderInt("Multi-point scale", &g_config.i[XOR("aimbot_multipoint_scale")], 0, 100, "%d%%");

			if (g_config.m[XOR("aimbot_target_multipoint")][1] || g_config.m[XOR("aimbot_target_multipoint")][2])
				ui::SliderInt("Body-point scale", &g_config.i[XOR("aimbot_body_scale")], 0, 100, "%d%%");

			ui::MultiSelect("Prefer baim", &g_config.m[XOR("aimbot_prefer_baim")], { "Always", "Lethal", "Lethal x2", "Fake", "In air"});
			ui::MultiSelect("Only baim", &g_config.m[XOR("aimbot_only_baim")], { "Always", "Lethal", "Fake", "In air"});

			if (g_config.m[XOR("aimbot_only_baim")][1])
				ui::SliderInt("##only_baim_health", &g_config.i[XOR("aimbot_only_baim_health")], 0, 100, "%dhp");

			ui::Checkbox("Ignore limbs", &g_config.b[XOR("aimbot_ignore_limbs")]);
			ui::SliderInt("Minimum damage", &g_config.i[XOR("aimbot_minimal_dmg")], 0, 100, "%dhp");
			//ui::Checkbox("Minimum damage scaled on health", &g_config.b[XOR("aimbot_minimal_dmg_scale")]);

			ui::SliderInt("Minimum penetration damage", &g_config.i[XOR("aimbot_minimal_dmg_pen")], 0, 100, "%dhp");
			//ui::Checkbox("Minimum penetration damage scaled on health", &g_config.b[XOR("aimbot_minimal_dmg_pen_scale")]);

			draw_lua_items("ragebot", "target point selection");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(2));
		ui::BeginChild("Accuracy", child_size);
		{
			ui::Checkbox("Remove recoil", &g_config.b[XOR("aimbot_remove_recoil")]);
			ui::Checkbox("Remove spread", &g_config.b[XOR("aimbot_remove_spread")]);
			ui::SingleSelect("Auto scope", &g_config.i[XOR("aimbot_scope")], { "Off", "Allways", "Hitchance fail" }); dmt("aimbot_target_selection");
			ui::Checkbox("Auto stop", &g_config.b[XOR("aimbot_autostop")]);
			ui::Checkbox("Correct anti-aim", &g_config.b[XOR("aimbot_resolver")]);
			if (g_config.b[XOR("aimbot_resolver")])
			{
				ui::Checkbox("Override anti-aim", &g_config.b[XOR("aimbot_override")]);
				ui::Keybind("##aimbot_override", &g_config.i[XOR("aimbot_override_key")], &g_config.i[XOR("aimbot_override_key_style")]);
			}
			ui::Checkbox("Predict fake-lag", &g_config.b[XOR("aimbot_lagfix")]);
			ui::SliderInt("Hitchance", &g_config.i[XOR("aimbot_hitchance")], 0, 100, "%d%%");
			draw_lua_items("ragebot", "accuracy");
		}
		ui::EndChild();
	}

	if (this->m_nCurrentTab == 1) {
		ui::SetNextWindowPos(calculateChildWindowPositionDouble(0));
		ui::BeginChild("Anti-aimbot angles", child_size_d);
		{
			ui::Checkbox("Enable", &g_config.b[XOR("antiaim_enable")]);
			if (g_config.b[XOR("antiaim_enable")])
			{
				ui::Checkbox("Edge", &g_config.b[XOR("antiaim_edge")]); dmt("antiaim_edge");
				ui::SingleSelect("Pitch", &g_config.i[XOR("antiaim_pich")], { "Off", "Down", "Up", "Random", "Ideal" }); dmt("antiaim_pich");
				ui::SingleSelect("Yaw base", &g_config.i[XOR("antiaim_yaw_base")], { "Off", "Static", "Away crosshair", "Away distance" }); dmt("antiaim_yaw_base");
				ui::SingleSelect("Yaw", &g_config.i[XOR("antiaim_yaw")], { "Off", "Direction", "Jitter", "Rotate", "Random" }); dmt("antiaim_yaw");
				if (g_config.i[XOR("antiaim_yaw")] > 0)
					ui::SliderInt("##antiaim_yaw_add", &g_config.i[XOR("antiaim_yaw_add")], -180, 180, "%d°°"); // fixme

				if (g_config.i[XOR("antiaim_yaw")] == 2)
					ui::SliderInt("Jitter range", &g_config.i[XOR("antiaim_yaw_add")], 0, 180, XOR("%d°°"));

				else if (g_config.i[XOR("antiaim_yaw")] == 3)
				{
					ui::SliderInt("Rotate range", &g_config.i[XOR("antiaim_rotate_range")], 0, 360, XOR("%d°°"));
					ui::SliderInt("Rotate speed", &g_config.i[XOR("antiaim_rotate_speed")], 0, 100);
				}
				
				ui::SingleSelect("Body yaw", &g_config.i[XOR("antiaim_body_yaw")], { XOR("Off"), XOR("Left"), XOR("Right"), XOR("Opposite"), XOR("Z") }); dmt(XOR("antiaim_body_yaw"));

				ui::SingleSelect("Fake yaw", &g_config.i[XOR("antiaim_fake_yaw")], { XOR("Off"), XOR("Default"), XOR("Relative"), XOR("Jitter"), XOR("Rotate"), XOR("Random"), XOR("Local view") }); dmt(XOR("antiaim_fake_yaw"));
				if (g_config.i[XOR("antiaim_fake_yaw")] == 2)
					ui::SliderInt("##antiaim_relative_yaw", &g_config.i[XOR("antiaim_relative_yaw")], 0, 180, XOR("%d°°")); dmt(XOR("antiaim_relative_yaw"));
				

				ui::SingleSelect("Yaw direction", &g_config.i[XOR("antiaim_yaw_direction")], { XOR("Auto"), XOR("Backwards"), XOR("Left"), XOR("Right"), XOR("Custom") }); dmt(XOR("antiaim_yaw_direction"));
				if (g_config.i[XOR("antiaim_yaw_direction")] == 4)
					ui::SliderInt("##antiaim_relative_yaw", &g_config.i[XOR("antiaim_yaw_direction_custom")], 0, 360, XOR("%d°°")); dmt(XOR("antiaim_yaw_direction_custom"));

				ui::Checkbox("Lock direction", &g_config.b[XOR("antiaim_lockdir")]); dmt("antiaim_lockdir");

				ui::Checkbox("Fakewalk", &g_config.b[XOR("antiaim_fakewalk")]); dmt("antiaim_fakewalk");
				ui::Keybind("##antiaim_fakewalk_key", &g_config.i["antiaim_fakewalk_key"], &g_config.i["antiaim_fakewalk_key_style"]);
			}
			draw_lua_items("antiaims", "anti_aimbot_angles");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPositionDouble(1));
		ui::BeginChild("Fake-lag settings", child_size_d);
		{
			ui::Checkbox("Enable", &g_config.b[XOR("fakelag_enable")]);
			if (g_config.b[XOR("fakelag_enable")])
			{
				ui::MultiSelect("Triggers", &g_config.m[XOR("fakelag_trigger")], { XOR("Move"), XOR("Air"), XOR("Crouch") });
				ui::SingleSelect("Mode", &g_config.i[XOR("fakelag_mode")], { XOR("Max"), XOR("Break"), XOR("Random"), XOR("Break step") }); dmt(XOR("fakelag_move"));
				ui::SliderInt("Limit", &g_config.i[XOR("fakelag_limit")], 0, 16, XOR("%dt")); dmt(XOR("fakelag_limit"));
				ui::Checkbox("On land", &g_config.b[XOR("fakelag_onland")]); dmt("fakelag_onland");
			}
			draw_lua_items("anti-aims", "fake-lag settings");
		}
		ui::EndChild();

	}

	// визуалы *-*
	if (this->m_nCurrentTab == 2) {
		ui::SetNextWindowPos(calculateChildWindowPosition(0));
		ui::BeginChild("Player ESP", child_size);
		{
			ui::Checkbox("Dormant", &g_config.b[XOR("esp_dormant")]);

			ui::Checkbox("Bounding box", &g_config.b[XOR("esp_box")]);
			ui::ColorEdit4("##esp_box_col", g_config.c[XOR("esp_box_col")]); dmt("esp_box_col");

			ui::Checkbox("Name", &g_config.b[XOR("esp_name")]);
			ui::ColorEdit4("##esp_name_col", g_config.c[XOR("esp_name_col")]); dmt("esp_name_col");

			ui::Checkbox("Health", &g_config.b[XOR("esp_health")]);

			ui::MultiSelect("Weapon", &g_config.m[XOR("esp_weapon")], { XOR("Text"), XOR("Icon"), }); dmt("esp_weapon");
			ui::ColorEdit4("##esp_weapon_col", g_config.c[XOR("esp_weapon_col")]); dmt("esp_weapon_col");

			ui::Checkbox("Ammo bar", &g_config.b[XOR("esp_ammo")]); dmt("esp_ammo");
			ui::ColorEdit4("##esp_ammo_col", g_config.c[XOR("esp_ammo_col")]); dmt("esp_ammo_col");

			ui::Checkbox("Glow", &g_config.b[XOR("esp_glow")]); dmt("esp_glow");
			ui::ColorEdit4("##esp_glow_col", g_config.c[XOR("esp_glow_col")]); dmt("esp_glow_col");

			ui::MultiSelect("Flags", &g_config.m[XOR("esp_flags")], { XOR("Money"),  XOR("Armor"),  XOR("Scoped"),  XOR("Flashed"),  XOR("Reload"),  XOR("Bomb") }); dmt("esp_flags");

			ui::MultiSelect("Skeleton", &g_config.m[XOR("esp_skeleton")], { XOR("Normal"), XOR("History") });
			ui::ColorEdit4("##esp_skeleton_col", g_config.c[XOR("esp_skeleton_col")]); dmt("esp_skeleton_col");

			ui::Checkbox("Lby bar", &g_config.b[XOR("esp_lby_bar")]); dmt("esp_lby_bar");
			ui::ColorEdit4("##esp_lby_bar_col", g_config.c[XOR("esp_lby_bar_col")]); dmt("esp_lby_bar_col");

			ui::Checkbox("Ping bar", &g_config.b[XOR("esp_ping_bar")]); dmt("esp_ping_bar");
			ui::ColorEdit4("##esp_ping_bar_col", g_config.c[XOR("esp_ping_bar_col")]); dmt("esp_ping_bar_col");

			ui::Checkbox("Out of FOV arrows", &g_config.b[XOR("esp_oof")]); dmt("esp_oof");
			ui::ColorEdit4("##esp_oof_col", g_config.c[XOR("esp_oof_col")]); dmt("esp_oof_col");
			if (g_config.b[XOR("esp_oof")])
			{
				ui::SliderInt("##oof_size", &g_config.i[XOR("esp_oof_size")], 0, 140, "%dpx");
				ui::SliderInt("##oof_pos", &g_config.i[XOR("esp_oof_pos")], 0, 100, "%d%%");
			}

			ui::Checkbox("Player", &g_config.b[XOR("chams_enemy")]); dmt("chams_enemy");
			if (g_config.b[XOR("chams_enemy")])
			{
				ui::MultiSelect("##chams_enemy_mat", &g_config.m["chams_enemy_mat"], { XOR("Material"), XOR("Flat") });
				ui::ColorEdit4("##chams_enemy_col", g_config.c[XOR("chams_enemy_col")]); dmt("chams_enemy_col");

				ui::MultiSelect("##chams_enemy_mat_xqz", &g_config.m["chams_enemy_mat_xqz"], { XOR("Material"), XOR("Flat") });
				ui::ColorEdit4("##chams_enemy_col_xqz", g_config.c[XOR("chams_enemy_col_xqz")]); dmt("chams_enemy_col_xqz");
			}


			ui::Checkbox("Teamate", &g_config.b[XOR("chams_team")]); dmt("chams_team");
			ui::ColorEdit4("##chams_team_col", g_config.c[XOR("chams_team_col")]); dmt("chams_team_col");
			if (g_config.b[XOR("chams_team")])
			{
				ui::MultiSelect("##chams_team_mat", &g_config.m["chams_team_mat"], { XOR("Material"), XOR("Flat") });
			}


			ui::Checkbox("Local", &g_config.b[XOR("chams_local")]); dmt("chams_local");
			ui::ColorEdit4("##chams_local_col", g_config.c[XOR("chams_local_col")]); dmt("chams_local_col");
			if (g_config.b[XOR("chams_local")])
			{
				ui::MultiSelect("##chams_local_mat", &g_config.m["chams_local_mat"], { XOR("Material"), XOR("Flat") });
			}

			ui::Checkbox("History", &g_config.b[XOR("chams_history")]); dmt("chams_history");
			ui::ColorEdit4("##chams_history_col", g_config.c[XOR("chams_history_col")]); dmt("chams_history");
			if (g_config.b[XOR("chams_history")])
			{

			}

			draw_lua_items("visuals", "player esp");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(1));
		ui::BeginChild("Other", child_size);
		{
			ui::Checkbox("Radar", &g_config.b[XOR("other_radar")]); dmt(XOR("other_radar"));
			ui::MultiSelect("Dropped weapon", &g_config.m[XOR("other_dropped_weapon")], { XOR("Text"), XOR("Ammo") }); dmt(XOR("other_dropped_weapon"));
			ui::ColorEdit4("##dropped_weapon_col", g_config.c[XOR("other_dropped_weapon_col")]); dmt(XOR("other_grenades"));

			ui::MultiSelect("Grenades", &g_config.m[XOR("other_grenades")], { XOR("Text"), XOR("Timer"), }); dmt(XOR("other_grenades"));
			ui::ColorEdit4("##other_grenades_col", g_config.c[XOR("other_grenades")]); dmt(XOR("other_grenades"));

			ui::Checkbox("Hostage", &g_config.b[XOR("other_hostage")]); dmt(XOR("other_hostage"));
			ui::Checkbox("Bomb", &g_config.b[XOR("other_bomb")]); dmt(XOR("other_bomb"));

			ui::Checkbox("Crosshair", &g_config.b[XOR("other_crosshair")]); dmt(XOR("other_crosshair"));

			ui::Checkbox("Hitmarker", &g_config.b[XOR("other_hitmarker")]); dmt(XOR("other_hitmarker"));
			ui::Checkbox("Spectators", &g_config.b[XOR("other_spectators")]); dmt(XOR("other_spectators"));
			ui::MultiSelect("Indicators", &g_config.m[XOR("other_indicators")], { XOR("LC"),XOR("LBY"),XOR("PING"), XOR("BAIM")}); dmt(XOR("other_indicators"));
			//ui::Checkbox("Spread crosshair", &g_config.b[XOR("other_spread_crosshair")]); dmt(XOR("other_spread_crosshair"));
			ui::ColorEdit4("##other_spread_crosshair", g_config.c[XOR("other_spread_crosshair_col")]); dmt("other_spread_crosshair_col");

			ui::Checkbox("Penetration reticle", &g_config.b[XOR("other_penetration_reticle")]); dmt(XOR("other_penetration_reticle"));

			ui::Checkbox("Grenade prediction", &g_config.b[XOR("other_grenade_predict")]); dmt(XOR("other_grenade_predict"));
			ui::ColorEdit4("##other_grenade_predict_col", g_config.c[XOR("other_grenade_predict_col")]); dmt("other_grenade_predict_col");

			draw_lua_items("visuals", "effects");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(2));
		ui::BeginChild("Effects", child_size);
		{
			ui::SingleSelect("Remove smoke grenades", &g_config.i[XOR("effects_remove_smoke")], {XOR("Off"), XOR("Remove"), XOR("Wireframe") }); dmt(XOR("effects_remove_smoke"));
			ui::Checkbox("Remove flash", &g_config.b[XOR("effects_remove_flash")]); dmt(XOR("effects_remove_flash"));
			ui::Checkbox("Remove teamates", &g_config.b[XOR("effects_remove_team")]); dmt(XOR("effects_remove_team"));
			ui::Checkbox("Remove scope overlay", &g_config.b[XOR("effects_remove_scope")]); dmt(XOR("effects_remove_scope"));
			ui::Checkbox("Remove visual recoil", &g_config.b[XOR("effects_remove_recoil")]); dmt(XOR("effects_remove_recoil"));
			ui::Checkbox("Remove fog", &g_config.b[XOR("effects_remove_fog")]); dmt(XOR("effects_remove_fog"));
			ui::SingleSelect("World modulation", &g_config.i[XOR("effects_worldmodulation")], { "Off", "Night mode", "Full bright" }); dmt(XOR("effects_worldmodulation"));
			if (g_config.i[XOR("effects_worldmodulation")] == 1)
				ui::SliderInt("##darknes", &g_config.i[XOR("effects_nightmode_value")], 0, 100, "%d%%"); dmt(XOR("effects_nightmode_value"));
			
			ui::SliderInt("Transperent props", &g_config.i[XOR("effects_transperent_value")], 0, 100, "%d%%"); dmt(XOR("effects_transperent_value"));

			ui::Checkbox("Force thirdperson", &g_config.b[XOR("effects_thirdperson")]); dmt(XOR("effects_thirdperson"));
			ui::Keybind("##effects_thirdperson_key", &g_config.i[XOR("effects_thirdperson_key")], &g_config.i[XOR("effects_thirdperson_key_style")]); dmt("effects_thirdperson_key | effects_thirdperson_key_style");
			if (g_config.b[XOR("effects_thirdperson")])
				ui::SliderInt("##thirdpreson_distance", &g_config.i[XOR("effects_thirdperson_dst")], 0, 200, "%d%%"); dmt(XOR("effects_thirdperson_dst"));

			ui::Checkbox("Bullet tracer", &g_config.b[XOR("effects_bullettrace")]); dmt(XOR("effects_bullettrace"));
			ui::ColorEdit4("##effects_bullettrace_col", g_config.c[XOR("effects_bullettrace_col")]); dmt(XOR("effects_bullettrace_col"));
			if (g_config.b[XOR("effects_bullettrace")])
				ui::SliderInt("##effects_bullettrace_time", &g_config.i[XOR("effects_bullettrace_time")], 0, 10, "%ds"); dmt(XOR("effects_bullettrace_time"));

			ui::Checkbox("Kill feed", &g_config.b[XOR("effects_killfeed")]);


			ui::Checkbox("FOV", &g_config.b[XOR("effects_fov")]);
			if (g_config.b[XOR("effects_fov")])
			{
				ui::Checkbox("FOV when scoped", &g_config.b[XOR("effects_fov_scoped")]);
				ui::SliderInt("##effects_fov_value", &g_config.i[XOR("effects_fov_value")], 0, 130, "%d°°");
			}


			ui::Checkbox("Viewmodel fov", &g_config.b[XOR("effects_viewmodel_fov")]);
			if (g_config.b[XOR("effects_viewmodel_fov")])
				ui::SliderInt("##effects_viewmodel_fov_value", &g_config.i[XOR("effects_viewmodel_fov_value")], 0, 130, "%d °");
			

			draw_lua_items("visuals", "other");
		}
		ui::EndChild();
	}

	if (this->m_nCurrentTab == 3) {
		ui::SetNextWindowPos(calculateChildWindowPosition(0));
		ui::BeginChild("Miscellaneous", child_size);
		{
			ui::Checkbox("Bunnyhop", &g_config.b["misc_bhop"]);
			ui::Checkbox("Autostrafe", &g_config.b["misc_strafe"]);
			ui::Checkbox("Duck in air", &g_config.b["misc_duckinair"]);


			ui::Checkbox("C-strafer", &g_config.b["misc_cstrafe"]);
			ui::Keybind("##misc_cstrafe_key", &g_config.i["misc_cstrafe_key"], &g_config.i["misc_cstrafe_key_style"]);


			ui::Checkbox("A-strafer", &g_config.b["misc_astrafe"]);
			ui::Keybind("##misc_astrafe_key", &g_config.i["misc_astrafe_key"], &g_config.i["misc_astrafe_key_style"]);


			ui::Checkbox("Z-strafer", &g_config.b["misc_zstrafe"]);
			ui::Keybind("##misc_zstrafe_key", &g_config.i["misc_zstrafe_key"], &g_config.i["misc_zstrafe_key_style"]);
			if (g_config.b["misc_zstrafe"])
			{
				ui::SliderInt("##misc_zstrafe_freq", &g_config.i["misc_zstrafe_freq"], 1, 100, "%dhz");
				ui::SliderInt("##misc_zstrafe_dist", &g_config.i["misc_zstrafe_dist"], 1, 100, "%d%%");
			}

			ui::Checkbox("Clan-tag", &g_config.b[XOR("misc_clantag")]);
			ui::MultiSelect("Notifications", &g_config.m[XOR("misc_notifications")], { XOR("Hit"), XOR("Purchase"), XOR("Hurt"), XOR("Bomb") });

			ui::Checkbox("Fake-ping", &g_config.b[XOR("misc_fakeping")]);
			ui::Keybind("##fakeping_key", &g_config.i["fakeping_key"], &g_config.i["fakeping_key_style"]);
			if (g_config.b[XOR("misc_fakeping")])
				ui::SliderInt("##misc_fakeping_value", &g_config.i["misc_fakeping"], 0, 800, "%dms");

			ui::Checkbox("Ragdoll Force", &g_config.b[XOR("misc_ragdol")]);
			ui::Checkbox("Unlock inventory", &g_config.b[XOR("misc_unlockinv")]);

			draw_lua_items("misc", "miscellaneous");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(1));
		ui::BeginChild("Cheat settings", child_size);
		{
			ui::Text("Menu key");
			ui::Keybind("menukey", &g_config.i[XOR("misMenukey")]); dmt("misMenukey");
			ui::Text("Menu color");
			ui::ColorEdit4("##menucolor", g_config.c[XOR("menu_color")]); dmt("menu_color");
			ui::Checkbox("Anti-untrasted", &g_config.b[XOR("anti_untrasted")]);

			ui::SingleSelect("Preset", &g_config.i["_preset"], g_config.presets); dmt("_preset");
			ui::Keybind("##presetkey", &g_config.i["_preset_" + std::to_string(g_config.i["_preset"])]); dmt("_preset_" + std::to_string(g_config.i["_preset"]));

			if (ui::Button("Load"))
				g_config.load();

			if (ui::Button("Save"))
				g_config.save();

			if (ui::Button("Open settings folder"))
				ShellExecuteA(0, "open", "C:/youlense", NULL, NULL, SW_NORMAL);

			draw_lua_items("misc", "cheat settings");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPosition(2));
		ui::BeginChild("Lua extensions", child_size);
		{
			ui::Checkbox("Developer mode", &g_config.b["lua_devmode"]); dmt("lua_devmode");
			if (ui::Button("Refresh scripts"))
				g_lua.refresh_scripts();
			if (ui::Button("Reload active scripts"))
				g_lua.reload_all_scripts();
			if (ui::Button("Unload all"))
				g_lua.unload_all_scripts();

			ui::ListBoxHeader("##urnn", ImVec2(0, 160));
			{
				for (auto s : g_lua.scripts)
				{
					if (ui::Selectable(s.c_str(), g_lua.loaded.at(g_lua.get_script_id(s)), NULL, ImVec2(0, 0))) {
						auto scriptId = g_lua.get_script_id(s);
						if (g_lua.loaded.at(scriptId))
							g_lua.unload_script(scriptId);
						else
							g_lua.load_script(scriptId);
					}
				}
			}
			ui::ListBoxFooter();
			draw_lua_items("misc", "lua extensions");
		}
		ui::EndChild();
	}

	if (this->m_nCurrentTab == 4)
	{
		ui::SetNextWindowPos(calculateChildWindowPositionDouble(0));
		ui::BeginChild("Knife & gloves", child_size_d);
		{
			ui::Checkbox("Enable", &g_config.b["skins_enable"]);
			if (g_config.b["skins_enable"])
			{
				ui::SingleSelect("Knife model", &g_config.i["skins_knife"], { XOR("Off"), XOR("Bayonet"), XOR("Bowie"), XOR("Butterfly"),
					XOR("Falchion"), XOR("Flip"), XOR("Gut"), XOR("Huntsman"), XOR("Karambit"), XOR("M9 bayonet"), XOR("Daggers") });

				ui::SingleSelect("Glove model", &g_config.i["skins_glove"], { XOR("Off"), XOR("Bloodhound"), XOR("Sport"),
					XOR("Driver"), XOR("Handwraps"), XOR("Moto"), XOR("Specialist") });


				if (g_config.i["skins_glove"] > 0)
				{
					std::vector<const char*> glove_skins = {};
					switch (g_config.i["skins_glove"]) {
					case 1: glove_skins = { XOR("Emerald"), XOR("Mangrove"), XOR("Rattler"), XOR("Case Hardened") }; break;
					case 2: glove_skins = { XOR("Vice"), XOR("Omega"), XOR("Amphibious"), XOR("Bronze Morph") }; break;
					case 3: glove_skins = { XOR("Overtake"), XOR("Racing Green"), XOR("King Snake"), XOR("Imperial Plaid") }; break;
					case 4: glove_skins = { XOR("Cobalt Skulls"), XOR("Overprint"), XOR("Duct Tape"), XOR("Arboreal") }; break;
					case 5: glove_skins = { XOR("Turtle"), XOR("Transport"), XOR("Polygon"), XOR("POW!") }; break;
					case 6: glove_skins = { XOR("Crimson Web"), XOR("Buckshot"), XOR("Fade"), XOR("Mogul") }; break;
					default:
						break;
					} 

					ui::SingleSelect("##glove_skin", &g_config.i["skins_glove_skin"], glove_skins);
				}
			}
			draw_lua_items("skins", "knife_and_gloves");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPositionDouble(1));
		ui::BeginChild("Skins", child_size_d);
		{
			if (g_config.b["skins_enable"])
			{

			}
			draw_lua_items("skins", "weapon skins");
		}

		ui::EndChild();
	}

	if (this->m_nCurrentTab == 5) {
		
		ui::SetNextWindowPos(calculateChildWindowPositionDouble(0));
		ui::BeginChild("Player list", child_size_d);
		{
			draw_lua_items("players", "player list");
		}
		ui::EndChild();

		ui::SetNextWindowPos(calculateChildWindowPositionDouble(1));
		ui::BeginChild("Override settings", child_size_d);
		{
			draw_lua_items("players", "override settings");
		}

		ui::EndChild();
	}

	ui::End();
}

void Menu::draw_end() {
	if (!this->m_bInitialized)
		return;

	ui::EndFrame();
	ui::Render();
	ImGui_ImplDX9_RenderDrawData(ui::GetDrawData());
}

bool Menu::is_menu_initialized() {
	return this->m_bInitialized;
}

bool Menu::is_menu_opened() {
	return this->m_bIsOpened;
}

void Menu::set_menu_opened(bool v) {
	this->m_bIsOpened = v;
}

IDirect3DTexture9* Menu::get_texture_data() {
	return this->m_pTexture;
}

ImColor Menu::get_accent_color() {
	return ImColor(
		g_config.c[XOR("menu_color")][0],
		g_config.c[XOR("menu_color")][1],
		g_config.c[XOR("menu_color")][2],
		ui::GetStyle().Alpha
	);
}