#include "clua.h"

void lua_panic(sol::optional<std::string> message) {

	if (message) {
		std::string m = message.value();
		MessageBoxA(0, m.c_str(), ("Lua: panic state"), MB_APPLMODAL | MB_OK);
	}
}

// ----- lua functions -----

int extract_owner(sol::this_state st) {
	sol::state_view lua_state(st);
	sol::table rs = lua_state["debug"]["getinfo"](2, "S");
	std::string source = rs["source"];
	std::string filename = std::filesystem::path(source.substr(1)).filename().string();
	return g_lua.get_script_id(filename);
}

namespace ns_config {
	/*
	config.get(key)
	Returns value of given key or nil if key not found.
	*/
	std::tuple<sol::object, sol::object, sol::object, sol::object> get(sol::this_state s, std::string key) {
		std::tuple<sol::object, sol::object, sol::object, sol::object> retn = std::make_tuple(sol::nil, sol::nil, sol::nil, sol::nil);

		for (auto kv : g_config.b)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		for (auto kv : g_config.c)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, (int)(kv.second[0] * 255)), sol::make_object(s, (int)(kv.second[1] * 255)), sol::make_object(s, (int)(kv.second[2] * 255)), sol::make_object(s, (int)(kv.second[3] * 255)));

		for (auto kv : g_config.f)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		for (auto kv : g_config.i)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		for (auto kv : g_config.m)
			if (kv.first == key)
				retn = std::make_tuple(sol::make_object(s, kv.second), sol::nil, sol::nil, sol::nil);

		return retn;
	}

	/*
	config.set(key, value)
	Sets value for key
	*/
	void set_bool(std::string key, bool v) {
		g_config.b[key] = v;
	}

	void set_float(std::string key, float v) {
		if (ceilf(v) != v)
			g_config.f[key] = v;
		else
			g_config.i[key] = (int)v;
	}

	void set_color(std::string key, int r, int g, int b, int a) {
		g_config.c[key][0] = r / 255.f;
		g_config.c[key][1] = g / 255.f;
		g_config.c[key][2] = b / 255.f;
		g_config.c[key][3] = a / 255.f;
	}

	void set_multiselect(std::string key, int pos, bool e) {
		g_config.m[key][pos] = e;
	}

	void set_int(std::string key, int value) {
		g_config.i[key] = value;
	}

	/*
	config.load()
	Loads selected config
	*/
	void load() {
		g_config.load();
	}

	/*
	config.save()
	Saves selected config

	*/
	void save() {
		g_config.save();
	}
}
namespace ns_cheat {
	void set_event_callback(sol::this_state s, std::string eventname, sol::function func) {
		sol::state_view lua_state(s);
		sol::table rs = lua_state["debug"]["getinfo"](2, ("S"));
		std::string source = rs["source"];
		std::string filename = std::filesystem::path(source.substr(1)).filename().string();

		g_lua_hook.registerHook(eventname, g_lua.get_script_id(filename), func);

		g_notify.add(filename + ": subscribed to event " + eventname  +"\n");
	}

	void run_script(std::string scriptname) {
		int scrid = g_lua.get_script_id(scriptname);
		if (scrid < 0)
			return;

		g_lua.load_script(scrid);
	}

	void reload_active_scripts() {
		g_lua.reload_all_scripts();
	}
}
namespace ns_models {
	/*
	models.get_studio_model(mdl)
	Returns studio model of mdl
	*/
	studiohdr_t* get_studio_model(model_t* mdl) {
		return g_csgo.m_model_info->GetStudioModel(mdl);
	}

	/*
	models.get_model_index(name)
	Returns model index of given name
	*/
	int get_model_index(std::string name) {
		return g_csgo.m_model_info->GetModelIndex(name.c_str());
	}
}
namespace ns_engine {
	std::tuple<int, int> get_screen_size() {
		int w, h;
		g_csgo.m_engine->GetScreenSize(w, h);
		return std::make_tuple(w, h);
	}

	void client_cmd(std::string cmd) {
		g_csgo.m_engine->ExecuteClientCmd(cmd.c_str());
	}

	player_info_t get_player_info(int ent) {
		player_info_t p;
		g_csgo.m_engine->GetPlayerInfo(ent, &p);
		return p;
	}

	int get_player_for_user_id(int userid) {
		return g_csgo.m_engine->GetPlayerForUserID(userid);
	}

	int get_local_player_index() {
		return g_csgo.m_engine->GetLocalPlayer();
	}

	float get_last_timestamp() {
		return g_csgo.m_engine->GetLastTimestamp();
	}

	ang_t get_view_angles() {
		ang_t va;
		g_csgo.m_engine->GetViewAngles(va);
		return va;
	}

	void set_view_angles(ang_t va) {
		g_csgo.m_engine->SetViewAngles(va);
	}

	int get_max_clients() {
		return g_csgo.m_globals->m_max_clients;
	}

	bool is_in_game() {
		return g_csgo.m_engine->IsInGame();
	}

	bool is_connected() {
		return g_csgo.m_engine->IsConnected();
	}


	INetChannel* get_net_channel_info() {
		return g_csgo.m_engine->GetNetChannelInfo();
	}

	bool is_paused() {
		return g_csgo.m_engine->IsPaused();
	}

	void execute_client_cmd(std::string cmd) {
		g_csgo.m_engine->ExecuteClientCmd(cmd.c_str());
	}
}
namespace ns_entity_list {
	Entity* get_client_entity(int idx) {
		return g_csgo.m_entlist->GetClientEntity(idx);
	}

	int get_highest_entity_index() {
		return g_csgo.m_entlist->GetHighestEntityIndex();
	}

	Entity* get_client_entity_from_handle(ULONG ent) {
		return g_csgo.m_entlist->GetClientEntityFromHandle(ent);
	}
}
namespace ns_utils {
	sol::table get_player_data(player_info_t& p) {
		sol::table t = g_lua.lua.create_table();
		t["name"] = std::string(p.m_name);
		t["guid"] = std::string(p.m_guid);
		t["userid"] = p.m_user_id;
		return t;
	}

	double clamp(double v, double mi, double ma) {
		return std::clamp(v, mi, ma);
	}

	vec2_t world_to_screen(vec3_t pos) {
		vec2_t scr;
		render::WorldToScreen(pos, scr);
		return scr;
	}
}
namespace ns_globals {
	float realtime() {
		return g_csgo.m_globals->m_realtime;
	}

	int framecount() {
		return g_csgo.m_globals->m_frame;
	}

	float absoluteframetime() {
		return g_csgo.m_globals->m_abs_frametime;
	}

	float curtime() {
		return g_csgo.m_globals->m_curtime;
	}

	float frametime() {
		return g_csgo.m_globals->m_frametime;
	}

	int maxclients() {
		return g_csgo.m_globals->m_max_clients;
	}

	int tickcount() {
		return g_csgo.m_globals->m_tick_count;
	}

	float tickinterval() {
		return g_csgo.m_globals->m_interval;
	}
}
namespace ns_trace {
	int get_point_contents(vec3_t pos, int mask) {
		return g_csgo.m_engine_trace->GetPointContents(pos, mask);
	}

	/*std::tuple<float, Player*> trace_ray(vec3_t from, vec3_t to, int mask) {
		Ray ray;
		ITraceFilter filter;
		trace_t trace;
		trace.start = from;
		trace.end = to;
		filter.pSkip1 = G::LocalPlayer;

		g_csgo.m_engine_trace->TraceRay(ray, mask, &filter, &trace);
		return std::make_tuple(trace.fraction, trace.m_pEnt);
	}*/
}
namespace ns_cvar {

	ConVar* find_var(std::string name) {
		return g_csgo.m_cvar->FindVar(FNV1a::get(name));
	}

	void console_color_print(Color color, sol::variadic_args args) {
		std::string txt = "";
		for (auto v : args)
			txt += v;

		g_csgo.m_cvar->ConsoleColorPrintf(color, txt.c_str());
	}

	void console_color_print_rgba(int r, int g, int b, int a, sol::variadic_args args) {
		std::string txt = "";
		for (auto v : args)
			txt += v;

		g_csgo.m_cvar->ConsoleColorPrintf(Color(r, g, b, a), txt.c_str());
	}

	void console_print(sol::variadic_args args) {
		std::string txt = "";
		for (auto v : args)
			txt += v;

		g_csgo.m_cvar->ConsoleColorPrintf(Color(255,255,255), txt.c_str());
	}

	void unlock_cvar(ConVar* var)
	{
		if (var)
		{
			var->m_flags &= ~FCVAR_DEVELOPMENTONLY;
			var->m_flags &= ~FCVAR_HIDDEN;
		}
	}

	void remove_callbacks(ConVar* var) {
		if (var)
			var->m_callbacks.RemoveAll();
	}

	float get_float(ConVar* var) {
		return var ? var->GetFloat() : 0.f;
	}

	int get_int(ConVar* var) {
		return var ? var->GetInt() : 0;
	}

	const char* get_string(ConVar* var) {
		return var ? var->GetString() : "";
	}

	void set_float(ConVar* var, float f, bool unlock = false) {
		if (var) var->SetValue(std::to_string(f).c_str());
	}

	void set_int(ConVar* var, int i, bool unlock = false) {
		if (var) var->SetValue(std::to_string(i).c_str());
	}

	void set_string(ConVar* var, const char* v, bool unlock = false) {
		if (var)
			var->SetValue(v);
	}
}
namespace ns_overlay {
	void add_box_overlay(vec3_t pos, vec3_t mins, vec3_t maxs, ang_t orientation, int r, int g, int b, int a, float duration) {
		g_csgo.m_debug_overlay->AddBoxOverlay(pos, mins, maxs, orientation, r, g, b, a, duration);
	}

	void add_text_overlay(vec3_t pos, int line_offset, float duration, sol::variadic_args txt) {
		std::string text = "";
		for (auto v : txt)
			text += v;

		g_csgo.m_debug_overlay->AddTextOverlay(pos, duration, text.c_str());
	}

	void add_capsule_overlay(vec3_t mins, vec3_t maxs, float pill, int r, int g, int b, int a, float duration) {
		g_csgo.m_debug_overlay->AddCapsuleOverlay(mins, maxs, pill, r, g, b, a, duration, 0, 0);
	}
}
namespace ns_beams {
	void draw_beam(Beam_t* beam) {
		g_csgo.m_beams->DrawBeam(beam);
	}

	Beam_t* create_points(BeamInfo_t beam) {
		return g_csgo.m_beams->CreateBeamPoints(beam);
	}
}

namespace ns_ui {
	std::string new_checkbox(sol::this_state s, std::string tab, std::string container, std::string label, std::string key, std::optional<bool> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_CHECKBOX;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;
		item.b_default = def.value_or(false);
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_slider_int(sol::this_state s, std::string tab, std::string container, std::string label, std::string key, int min, int max, std::optional<std::string> format, std::optional<int> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_SLIDERINT;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;
		item.i_default = def.value_or(0);
		item.i_min = min;
		item.i_max = max;
		item.format = format.value_or("%d");
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_slider_float(sol::this_state s, std::string tab, std::string container, std::string label, std::string key, float min, float max, std::optional<std::string> format, std::optional<float> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_SLIDERFLOAT;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;
		item.f_default = def.value_or(0.f);
		item.f_min = min;
		item.f_max = max;
		item.format = format.value_or("%.0f");
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_keybind(sol::this_state s, std::string tab, std::string container, std::string id, std::string key, std::optional<bool> allow_sc, std::optional<int> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_KEYBIND;
		item.script = extract_owner(s);
		item.label = id;
		item.key = key;
		item.allow_style_change = allow_sc.value_or(true);
		item.i_default = def.value_or(0);
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_text(sol::this_state s, std::string tab, std::string container, std::string label, std::string key) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_TEXT;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_singleselect(sol::this_state s, std::string tab, std::string container, std::string label, std::string key, std::vector<const char*> items, std::optional<int> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_SINGLESELECT;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;
		item.i_default = def.value_or(0);
		item.items = items;
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_multiselect(sol::this_state s, std::string tab, std::string container, std::string label, std::string key, std::vector<const char*> items, std::optional<std::map<int, bool>> def, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_MULTISELECT;
		item.script = extract_owner(s);
		item.label = label;
		item.key = key;
		item.m_default = def.value_or(std::map<int, bool> {});
		item.items = items;
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_colorpicker(sol::this_state s, std::string tab, std::string container, std::string id, std::string key, std::optional<int> r, std::optional<int> g, std::optional<int> b, std::optional<int> a, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_COLORPICKER;
		item.script = extract_owner(s);
		item.label = id;
		item.key = key;
		item.c_default[0] = r.value_or(255) / 255.f;
		item.c_default[1] = g.value_or(255) / 255.f;
		item.c_default[2] = b.value_or(255) / 255.f;
		item.c_default[3] = a.value_or(255) / 255.f;
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	std::string new_button(sol::this_state s, std::string tab, std::string container, std::string id, std::string key, std::optional<sol::function> cb) {
		std::transform(tab.begin(), tab.end(), tab.begin(), ::tolower);
		std::transform(container.begin(), container.end(), container.begin(), ::tolower);

		MenuItem_t item;
		item.type = MENUITEM_BUTTON;
		item.script = extract_owner(s);
		item.label = id;
		item.key = key;
		item.callback = cb.value_or(sol::nil);

		g_lua.menu_items[tab][container].push_back(item);
		return key;
	}

	void set_visibility(std::string key, bool v) {
		for (auto t : g_lua.menu_items) {
			for (auto c : t.second) {
				for (auto& i : c.second) {
					if (i.key == key)
						i.is_visible = v;
				}
			}
		}
	}

	void set_items(std::string key, std::vector<const char*> items) {
		for (auto t : g_lua.menu_items) {
			for (auto c : t.second) {
				for (auto& i : c.second) {
					if (i.key == key)
						i.items = items;
				}
			}
		}
	}

	void set_callback(std::string key, sol::function v) {
		for (auto t : g_lua.menu_items) {
			for (auto c : t.second) {
				for (auto& i : c.second) {
					if (i.key == key)
						i.callback = v;
				}
			}
		}
	}

	void set_label(std::string key, std::string v) {
		for (auto t : g_lua.menu_items) {
			for (auto c : t.second) {
				for (auto& i : c.second) {
					if (i.key == key)
						i.label = v;
				}
			}
		}
	}

	bool is_bind_active(std::string key) {
		return g_config.auto_check(key);
	}
}

namespace ns_surface {
	void set_texture(int id) {
		g_csgo.m_surface->DrawSetTexture(id);
	}

	void set_texture_rgba(int id, const unsigned char* rgba, int w, int h) {
		g_csgo.m_surface->DrawSetTextureRGBA(id, rgba, w, h);
	}

	void set_color(int r, int g, int b, int a) {
		g_csgo.m_surface->DrawSetColor(Color(r, g, b, a));
	}

	int create_texture() {
		return g_csgo.m_surface->CreateNewTextureID();
	}

	void draw_filled_rect(int x, int y, int x2, int y2) {
		g_csgo.m_surface->DrawFilledRect(x, y, x2, y2);
	}

	void draw_outlined_rect(int x, int y, int x2, int y2) {
		g_csgo.m_surface->DrawOutlinedRect(x, y, x2, y2);
	}

	std::tuple<int, int> get_text_size(int font, std::wstring text) {
		int w, h;
		g_csgo.m_surface->GetTextSize(font, text.c_str(), w, h);
		return std::make_tuple(w, h);
	}

	void draw_line(int x, int y, int x2, int y2) {
		g_csgo.m_surface->DrawLine(x, y, x2, y2);
	}

	void draw_outlined_circle(int x, int y, int radius, int segments) {
		g_csgo.m_surface->DrawOutlinedCircle(x, y, radius, segments);
	}

	int create_font(std::string fontname, int w, int h, int blur, int flags) {
		auto f = g_csgo.m_surface->CreateFont();
		g_csgo.m_surface->SetFontGlyphSet(f, fontname.c_str(), w, h, blur, 0, flags);
		return f;
	}

	void set_text_font(int font) {
		g_csgo.m_surface->DrawSetTextFont(font);
	}

	void set_text_color(int r, int g, int b, int a) {
		g_csgo.m_surface->DrawSetTextColor(Color(r, g, b, a));
	}

	void set_text_pos(int x, int y) {
		g_csgo.m_surface->DrawSetTextPos(x, y);
	}

	void draw_text(std::wstring str) {
		g_csgo.m_surface->DrawPrintText(str.c_str(), str.length());
	}

	void draw_textured_rect(int x, int y, int x2, int y2) {
		g_csgo.m_surface->DrawTexturedRect(x, y, x2, y2);
	}

	/*void indicator(std::string str, int r, int g, int b, int a) {
		Visuals->custom_inds.push_back({ str, CColor(r, g, b, a) });
	}*/

	void draw_filled_rect_fade(int x, int y, int x2, int y2, int alpha, int alpha2, bool horizontal) {
		g_csgo.m_surface->DrawFilledRectFade(x, y, x2, y2, alpha, alpha2, horizontal);
	}
}

// ----- lua functions -----

c_lua g_lua;

void c_lua::init() {
	this->lua = sol::state(sol::c_call<decltype(&lua_panic), &lua_panic>);
	this->lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::table, sol::lib::math, sol::lib::debug);

	this->lua["collectgarbage"] = sol::nil;
	this->lua["dofile"] = sol::nil;
	this->lua["load"] = sol::nil;
	this->lua["loadfile"] = sol::nil;
	this->lua["pcall"] = sol::nil;
	this->lua["print"] = sol::nil;
	this->lua["xpcall"] = sol::nil;
	this->lua["getmetatable"] = sol::nil;
	this->lua["setmetatable"] = sol::nil;
	this->lua["__nil_callback"] = []() {};

	this->lua["print"] = [](std::string s) { g_notify.add(s.c_str()); };
	this->lua["error"] = [](std::string s) { g_notify.add(s.c_str()); };

	this->lua.new_usertype<CUserCmd>("c_usercmd",
		"command_number", sol::readonly(&CUserCmd::m_command_number),
		"tick_count", sol::readonly(&CUserCmd::m_tick),
		"viewangles", &CUserCmd::m_view_angles,
		"aimdirection", &CUserCmd::m_aimdirection,
		"forwardmove", &CUserCmd::m_forward_move,
		"sidemove", &CUserCmd::m_side_move,
		"upmove", &CUserCmd::m_up_move,
		"buttons", &CUserCmd::m_buttons,
		"impulse", sol::readonly(&CUserCmd::m_impulse),
		"weaponselect", &CUserCmd::m_weapon_select,
		"weaponsubtype", sol::readonly(&CUserCmd::m_weapon_subtype),
		"random_seed", sol::readonly(&CUserCmd::m_random_seed),
		"mousedx", &CUserCmd::m_mousedx,
		"mousedy", &CUserCmd::m_mousedy,
		"hasbeenpredicted", sol::readonly(&CUserCmd::m_predicted)
		);
	this->lua.new_usertype<IGameEvent>("c_gameevent",
		"get_name", &IGameEvent::GetName,
		"is_reliable", &IGameEvent::IsReliable,
		"is_local", &IGameEvent::IsLocal,
		"is_empty", &IGameEvent::IsEmpty,
		"get_bool", &IGameEvent::GetBool,
		"get_int", &IGameEvent::GetInt,
		"get_uint64", &IGameEvent::GetUint64,
		"get_float", &IGameEvent::GetFloat,
		"get_string", &IGameEvent::GetString
		);
	this->lua.new_enum("HITBOXES",
		"HEAD", Hitboxes_t::HITBOX_HEAD,
		"NECK", Hitboxes_t::HITBOX_NECK,
		"PELVIS", Hitboxes_t::HITBOX_PELVIS,
		"BODY", Hitboxes_t::HITBOX_BODY,
		"THORAX", Hitboxes_t::HITBOX_THORAX,
		"CHEST", Hitboxes_t::HITBOX_CHEST,
		"UPPER_CHEST", Hitboxes_t::HITBOX_UPPER_CHEST,
		"RIGHT_THIGH", Hitboxes_t::HITBOX_R_THIGH,
		"LEFT_THIGH", Hitboxes_t::HITBOX_L_THIGH,
		"RIGHT_CALF", Hitboxes_t::HITBOX_R_CALF,
		"LEFT_CALF", Hitboxes_t::HITBOX_L_CALF,
		"RIGHT_FOOT", Hitboxes_t::HITBOX_R_FOOT,
		"LEFT_FOOT", Hitboxes_t::HITBOX_L_FOOT,
		"RIGHT_HAND", Hitboxes_t::HITBOX_R_HAND,
		"LEFT_HAND", Hitboxes_t::HITBOX_L_HAND,
		"RIGHT_UPPER_ARM", Hitboxes_t::HITBOX_R_UPPER_ARM,
		"RIGHT_FOREARM", Hitboxes_t::HITBOX_R_FOREARM,
		"LEFT_UPPER_ARM", Hitboxes_t::HITBOX_L_UPPER_ARM,
		"LEFT_FOREARM", Hitboxes_t::HITBOX_L_FOREARM
	);
	this->lua.new_enum("FONTFLAGS",
		"NONE", EFontFlags::FONTFLAG_NONE,
		"ITALIC", EFontFlags::FONTFLAG_ITALIC,
		"UNDERLINE", EFontFlags::FONTFLAG_UNDERLINE,
		"STRIKEOUT", EFontFlags::FONTFLAG_STRIKEOUT,
		"SYMBOL", EFontFlags::FONTFLAG_SYMBOL,
		"ANTIALIAS", EFontFlags::FONTFLAG_ANTIALIAS,
		"GAUSSIANBLUR", EFontFlags::FONTFLAG_GAUSSIANBLUR,
		"ROTARY", EFontFlags::FONTFLAG_ROTARY,
		"DROPSHADOW", EFontFlags::FONTFLAG_DROPSHADOW,
		"ADDITIVE", EFontFlags::FONTFLAG_ADDITIVE,
		"OUTLINE", EFontFlags::FONTFLAG_OUTLINE,
		"CUSTOM", EFontFlags::FONTFLAG_CUSTOM,
		"BITMAP", EFontFlags::FONTFLAG_BITMAP
	);
	this->lua.new_enum("BEAMFLAGS",
		"STARTENTITY", FBEAM_STARTENTITY,
		"ENDENTITY", FBEAM_ENDENTITY,
		"FADEIN", FBEAM_FADEIN,
		"FADEOUT", FBEAM_FADEOUT,
		"SINENOISE", FBEAM_SINENOISE,
		"SOLID", FBEAM_SOLID,
		"SHADEIN", FBEAM_SHADEIN,
		"SHADEOUT", FBEAM_SHADEOUT,
		"ONLYNOISEONCE", FBEAM_ONLYNOISEONCE,
		"NOTILE", FBEAM_NOTILE,
		"USE_HITBOXES", FBEAM_USE_HITBOXES,
		"STARTVISIBLE", FBEAM_STARTVISIBLE,
		"ENDVISIBLE", FBEAM_ENDVISIBLE,
		"ISACTIVE", FBEAM_ISACTIVE,
		"FOREVER", FBEAM_FOREVER,
		"HALOBEAM", FBEAM_HALOBEAM,
		"REVERSED", FBEAM_REVERSED
	);
	this->lua.new_enum("BEAMTYPES",
		"POINTS", TE_BEAMPOINTS,
		"SPRITE", TE_SPRITE,
		"DISK", TE_BEAMDISK,
		"CYLINDER", TE_BEAMCYLINDER,
		"FOLLOW", TE_BEAMFOLLOW,
		"RING", TE_BEAMRING,
		"SPLINE", TE_BEAMSPLINE,
		"RINGPOINT", TE_BEAMRINGPOINT,
		"LASER", TE_BEAMLASER,
		"TESLA", TE_BEAMTESLA
	);
	this->lua.new_usertype<BeamInfo_t>("c_beaminfo",
		sol::constructors<BeamInfo_t()>(),
		"type", &BeamInfo_t::m_nType,
		"start_ent", &BeamInfo_t::m_pStartEnt,
		"start_attachment", &BeamInfo_t::m_nStartAttachment,
		"end_ent", &BeamInfo_t::m_pEndEnt,
		"end_attachment", &BeamInfo_t::m_nEndAttachment,
		"start", &BeamInfo_t::m_vecStart,
		"end", &BeamInfo_t::m_vecEnd,
		"model_index", &BeamInfo_t::m_nModelIndex,
		"model_name", &BeamInfo_t::m_pszModelName,
		"halo_index", &BeamInfo_t::m_nHaloIndex,
		"halo_name", &BeamInfo_t::m_pszHaloName,
		"halo_scale", &BeamInfo_t::m_flHaloScale,
		"life", &BeamInfo_t::m_flLife,
		"width", &BeamInfo_t::m_flWidth,
		"end_width", &BeamInfo_t::m_flEndWidth,
		"fade_length", &BeamInfo_t::m_flFadeLength,
		"amplitude", &BeamInfo_t::m_flAmplitude,
		"brightness", &BeamInfo_t::m_flBrightness,
		"speed", &BeamInfo_t::m_flSpeed,
		"start_frame", &BeamInfo_t::m_nStartFrame,
		"framerate", &BeamInfo_t::m_flFrameRate,
		"red", &BeamInfo_t::m_flRed,
		"green", &BeamInfo_t::m_flGreen,
		"blue", &BeamInfo_t::m_flBlue,
		"renderable", &BeamInfo_t::m_bRenderable,
		"segments", &BeamInfo_t::m_nSegments,
		"flags", &BeamInfo_t::m_nFlags,
		"center", &BeamInfo_t::m_vecCenter,
		"start_radius", &BeamInfo_t::m_flStartRadius,
		"end_radius", &BeamInfo_t::m_flEndRadius
		);
	this->lua.new_usertype<Beam_t>("c_beam");
	this->lua.new_usertype<player_info_t>("c_playerinfo");
	this->lua.new_usertype<ConVar>("c_convar");
	this->lua.new_usertype<INetChannel>("c_netchannelinfo",
		"get_latency", &INetChannel::GetLatency,
		"get_avg_latency", &INetChannel::GetAvgLatency
		);
	this->lua.new_usertype<INetChannel>("c_netchannel",
		"out_sequence_nr", sol::readonly(&INetChannel::m_out_seq),
		"in_sequence_nr", sol::readonly(&INetChannel::m_in_seq),
		"out_sequence_nr_ack", sol::readonly(&INetChannel::m_out_seq_ack),
		"out_reliable_state", sol::readonly(&INetChannel::m_out_rel_state),
		"in_reliable_state", sol::readonly(&INetChannel::m_in_rel_state),
		"choked_packets", sol::readonly(&INetChannel::m_choked_packets)
		);
	this->lua.new_usertype<Color>("c_color",
		sol::constructors<Color(), Color(int, int, int), Color(int, int, int, int)>(),
		"r", &Color::r,
		"g", &Color::g,
		"b", &Color::b,
		"a", &Color::a
		);
	this->lua.new_usertype<vec2_t>("c_vec2_t",
		sol::constructors<vec2_t(), vec2_t(float, float), vec2_t(vec2_t)>(),
		"x", &vec2_t::x,
		"y", &vec2_t::y,
		"length", &vec2_t::length
		);
	this->lua.new_usertype<Vertex>("c_vertex",
		sol::constructors<Vertex(), Vertex(vec2_t), Vertex(vec2_t, vec2_t)>(),
		"init", &Vertex::init,
		"position", &Vertex::m_pos,
		"tex_coord", &Vertex::m_coord
		);
	this->lua.new_usertype<vec3_t>("c_vec3_t",
		sol::constructors<vec3_t(), vec3_t(float, float, float)>(),
		"x", &vec3_t::x,
		"y", &vec3_t::y,
		"z", &vec3_t::z,
		"length", &vec3_t::length,
		"length_sqr", &vec3_t::length_sqr,
		"length_2d", &vec3_t::length_2d,
		"length_2d_sqr", &vec3_t::length_2d_sqr,
		"dist_to", &vec3_t::dist_to,
		"cross_product", &vec3_t::cross,
		"normalize", &vec3_t::normalize,
		"normalize_angles", &vec3_t::normalized
		);
	this->lua.new_usertype<studiohdr_t>("c_studiohdr",
		"id", sol::readonly(&studiohdr_t::m_id),
		"version", sol::readonly(&studiohdr_t::m_version),
		"checksum", sol::readonly(&studiohdr_t::m_checksum),
		"length", sol::readonly(&studiohdr_t::m_length),
		"eyeposition", sol::readonly(&studiohdr_t::m_eye_pos),
		"illumposition", sol::readonly(&studiohdr_t::m_illum_pos),
		"hull_min", sol::readonly(&studiohdr_t::m_hull_mins),
		"hull_max", sol::readonly(&studiohdr_t::m_hull_maxs),
		"view_bbmin", sol::readonly(&studiohdr_t::m_view_mins),
		"view_bbmax", sol::readonly(&studiohdr_t::m_view_maxs),
		"flags", sol::readonly(&studiohdr_t::m_flags),
		"numbones", sol::readonly(&studiohdr_t::m_num_bones),
		"get_bone", &studiohdr_t::GetBone,
		"numhitboxsets", sol::readonly(&studiohdr_t::m_num_sets),
		"get_hitbox_set", &studiohdr_t::GetHitboxSet
		);
	this->lua.new_usertype<mstudiohitboxset_t>("c_studiohitboxset",
		"get_hitbox", &mstudiohitboxset_t::GetHitbox
		);
	this->lua.new_usertype<mstudiobbox_t>("c_studiobbox",
		"bone", sol::readonly(&mstudiobbox_t::m_bone),
		"group", sol::readonly(&mstudiobbox_t::m_group),
		"bbmin", sol::readonly(&mstudiobbox_t::m_mins),
		"bbmax", sol::readonly(&mstudiobbox_t::m_maxs),
		"radius", sol::readonly(&mstudiobbox_t::m_radius),
		"get_hitbox_name_id", &mstudiobbox_t::m_name_id
		);
	this->lua.new_usertype<model_t>("c_model");
	this->lua.new_usertype<CCSGOPlayerAnimState>("c_animstate",
		"entity", &CCSGOPlayerAnimState::m_player,
		"last_cs_anim_update_time", &CCSGOPlayerAnimState::m_time,
		"last_cs_anim_update_fc", &CCSGOPlayerAnimState::m_frame,
		"update_time_delta", &CCSGOPlayerAnimState::m_update_delta,
		"yaw", &CCSGOPlayerAnimState::m_eye_yaw,
		"pitch", &CCSGOPlayerAnimState::m_eye_pitch,
		"goal_feet_yaw", &CCSGOPlayerAnimState::m_goal_feet_yaw,
		"current_feet_yaw", &CCSGOPlayerAnimState::m_cur_feet_yaw,
		"fall_velocity", &CCSGOPlayerAnimState::m_fall_velocity,
		"m_speed", &CCSGOPlayerAnimState::m_speed,
		"m_land", &CCSGOPlayerAnimState::m_land,
		"m_ground", &CCSGOPlayerAnimState::m_ground
		);
	this->lua.new_usertype<Player>("c_baseentity",
		"is_player", &Player::IsPlayer,
		"get_abs_origin", &Player::GetAbsOrigin,
		"get_index", &Player::index,
		"get_spawn_time", &Player::m_flSpawnTime,
		"get_anim_state", &Player::m_PlayerAnimState
		);
	this->lua.new_usertype<ModelRenderInfo_t>("c_modelrenderinfo",
		"origin", sol::readonly(&ModelRenderInfo_t::m_origin),
		"angles", sol::readonly(&ModelRenderInfo_t::m_angles),
		"entity_index", sol::readonly(&ModelRenderInfo_t::m_index)
		);
	this->lua.new_usertype<CViewSetup>("c_viewsetup",
		"x", &CViewSetup::m_x,
		"x_old", &CViewSetup::m_old_x,
		"y", &CViewSetup::m_y,
		"y_old", &CViewSetup::m_old_y,
		"width", &CViewSetup::m_width,
		"width_old", &CViewSetup::m_old_width,
		"height", &CViewSetup::m_height,
		"height_old", &CViewSetup::m_old_height,
		"fov", &CViewSetup::m_fov,
		"viewmodel_fov", &CViewSetup::m_viewmodel_fov,
		"origin", &CViewSetup::m_origin,
		"angles", &CViewSetup::m_angles
		);

	auto config = this->lua.create_table();
	config["get"] = ns_config::get;
	config["set"] = sol::overload(ns_config::set_bool, ns_config::set_color, ns_config::set_float, ns_config::set_multiselect, ns_config::set_int);
	config["load"] = ns_config::load;
	config["save"] = ns_config::save;

	auto cheat = this->lua.create_table();
	cheat["set_event_callback"] = ns_cheat::set_event_callback;
	cheat["run_script"] = ns_cheat::run_script;
	cheat["reload_active_scripts"] = ns_cheat::reload_active_scripts;

	auto surface = this->lua.create_table();
	surface["create_font"] = ns_surface::create_font;
	surface["create_texture"] = ns_surface::create_texture;
	surface["draw_filled_rect"] = ns_surface::draw_filled_rect;
	surface["draw_line"] = ns_surface::draw_line;
	surface["draw_outlined_circle"] = ns_surface::draw_outlined_circle;
	surface["draw_outlined_rect"] = ns_surface::draw_outlined_rect;
	surface["draw_text"] = ns_surface::draw_text;
	surface["draw_textured_rect"] = ns_surface::draw_textured_rect;
	surface["get_text_size"] = ns_surface::get_text_size;
	//surface["indicator"] = ns_surface::indicator;
	surface["set_color"] = ns_surface::set_color;
	surface["set_texture"] = sol::overload(ns_surface::set_texture, ns_surface::set_texture_rgba);
	surface["set_text_color"] = ns_surface::set_text_color;
	surface["set_text_font"] = ns_surface::set_text_font;
	surface["set_text_pos"] = ns_surface::set_text_pos;
	surface["draw_filled_rect_fade"] = ns_surface::draw_filled_rect_fade;

	auto models = this->lua.create_table();
	models["get_model_index"] = ns_models::get_model_index;
	models["get_studio_model"] = ns_models::get_studio_model;

	auto engine = this->lua.create_table();
	engine["client_cmd"] = ns_engine::client_cmd;
	engine["execute_client_cmd"] = ns_engine::execute_client_cmd;
	engine["get_last_timestamp"] = ns_engine::get_last_timestamp;
	engine["get_local_player_index"] = ns_engine::get_local_player_index;
	engine["get_max_clients"] = ns_engine::get_max_clients;
	engine["get_net_channel_info"] = ns_engine::get_net_channel_info;
	engine["get_player_for_user_id"] = ns_engine::get_player_for_user_id;
	engine["get_player_info"] = ns_engine::get_player_info;
	engine["get_screen_size"] = ns_engine::get_screen_size;
	engine["get_view_angles"] = ns_engine::get_view_angles;
	engine["is_connected"] = ns_engine::is_connected;
	engine["is_in_game"] = ns_engine::is_in_game;
	engine["is_paused"] = ns_engine::is_paused;
	engine["set_view_angles"] = ns_engine::set_view_angles;

	auto entity_list = this->lua.create_table();
	entity_list["get_client_entity"] = ns_entity_list::get_client_entity;
	entity_list["get_client_entity_from_handle"] = ns_entity_list::get_client_entity_from_handle;
	entity_list["get_highest_entity_index"] = ns_entity_list::get_highest_entity_index;

	auto utils = this->lua.create_table();
	utils["get_player_data"] = ns_utils::get_player_data;
	utils["clamp"] = ns_utils::clamp;
	utils["world_to_screen"] = ns_utils::world_to_screen;

	auto globals = this->lua.create_table();
	globals["realtime"] = ns_globals::realtime;
	globals["framecount"] = ns_globals::framecount;
	globals["absoluteframetime"] = ns_globals::absoluteframetime;
	globals["curtime"] = ns_globals::curtime;
	globals["frametime"] = ns_globals::frametime;
	globals["maxclients"] = ns_globals::maxclients;
	globals["tickcount"] = ns_globals::tickcount;
	globals["tickinterval"] = ns_globals::tickinterval;

	auto trace = this->lua.create_table();
	trace["get_point_contents"] = ns_trace::get_point_contents;
	//trace["trace_ray"] = ns_trace::trace_ray;

	auto cvar = this->lua.create_table();
	cvar["console_color_print"] = sol::overload(ns_cvar::console_color_print, ns_cvar::console_color_print_rgba);
	cvar["console_print"] = ns_cvar::console_print;
	cvar["find_var"] = ns_cvar::find_var;
	cvar["get_float"] = ns_cvar::get_float;
	cvar["get_int"] = ns_cvar::get_int;
	cvar["set_float"] = ns_cvar::set_float;
	cvar["set_int"] = ns_cvar::set_int;
	cvar["get_string"] = ns_cvar::get_string;
	cvar["set_string"] = ns_cvar::set_string;
	cvar["unlock_cvar"] = ns_cvar::unlock_cvar;
	cvar["remove_callbacks"] = ns_cvar::remove_callbacks;

	auto overlay = this->lua.create_table();
	overlay["add_box_overlay"] = ns_overlay::add_box_overlay;
	overlay["add_capsule_overlay"] = ns_overlay::add_capsule_overlay;
	overlay["add_text_overlay"] = ns_overlay::add_text_overlay;

	auto beams = this->lua.create_table();
	beams["create_points"] = ns_beams::create_points;
	beams["draw_beam"] = ns_beams::draw_beam;

	auto ui = this->lua.create_table();
	ui["new_checkbox"] = ns_ui::new_checkbox;
	ui["new_colorpicker"] = ns_ui::new_colorpicker;
	ui["new_keybind"] = ns_ui::new_keybind;
	ui["new_multiselect"] = ns_ui::new_multiselect;
	ui["new_singleselect"] = ns_ui::new_singleselect;
	ui["new_slider_float"] = ns_ui::new_slider_float;
	ui["new_slider_int"] = ns_ui::new_slider_int;
	ui["new_text"] = ns_ui::new_text;
	ui["new_button"] = ns_ui::new_button;
	ui["set_callback"] = ns_ui::set_callback;
	ui["set_items"] = ns_ui::set_items;
	ui["set_label"] = ns_ui::set_label;
	ui["set_visibility"] = ns_ui::set_visibility;
	ui["is_bind_active"] = ns_ui::is_bind_active;

	this->lua["config"] = config;
	this->lua["cheat"] = cheat;
	this->lua["surface"] = surface;
	this->lua["models"] = models;
	this->lua["engine"] = engine;
	this->lua["entity_list"] = entity_list;
	this->lua["utils"] = utils;
	this->lua["globals"] = globals;
	this->lua["trace"] = trace;
	this->lua["cvar"] = cvar;
	this->lua["overlay"] = overlay;
	this->lua["beams"] = beams;
	this->lua["ui"] = ui;

	this->refresh_scripts();
	this->load_script(this->get_script_id("autorun.lua"));
}

void c_lua::load_script(int id) {
	if (id == -1)
		return;

	if (this->loaded.at(id))
		return;

	auto path = this->get_script_path(id);
	if (path == (""))
		return;

	this->lua.script_file(path, [](lua_State*, sol::protected_function_result result) {
		if (!result.valid()) {
			sol::error err = result;
			g_notify.add(err.what());
		}

		return result;
		});

	this->loaded.at(id) = true;
}

void c_lua::unload_script(int id) {
	if (id == -1)
		return;

	if (!this->loaded.at(id))
		return;

	std::map<std::string, std::map<std::string, std::vector<MenuItem_t>>> updated_items;
	for (auto i : this->menu_items) {
		for (auto k : i.second) {
			std::vector<MenuItem_t> updated_vec;

			for (auto m : k.second)
				if (m.script != id)
					updated_vec.push_back(m);

			updated_items[k.first][i.first] = updated_vec;
		}
	}
	this->menu_items = updated_items;

	g_lua_hook.unregisterHooks(id);
	this->loaded.at(id) = false;
}

void c_lua::reload_all_scripts() {
	for (auto s : this->scripts) {
		if (this->loaded.at(this->get_script_id(s))) {
			this->unload_script(this->get_script_id(s));
			this->load_script(this->get_script_id(s));
		}
	}
}

void c_lua::unload_all_scripts() {
	for (auto s : this->scripts)
		if (this->loaded.at(this->get_script_id(s)))
			this->unload_script(this->get_script_id(s));
}

void c_lua::refresh_scripts() {
	auto oldLoaded = this->loaded;
	auto oldScripts = this->scripts;

	this->loaded.clear();
	this->pathes.clear();
	this->scripts.clear();

	for (auto& entry : std::filesystem::directory_iterator("C:\\youlense\\lua")) {
		if (entry.path().extension() == (".lua")) {
			auto path = entry.path();
			auto filename = path.filename().string();

			bool didPut = false;
			for (int i = 0; i < oldScripts.size(); i++) {
				if (filename == oldScripts.at(i)) {
					this->loaded.push_back(oldLoaded.at(i));
					didPut = true;
				}
			}

			if (!didPut)
				this->loaded.push_back(false);

			this->pathes.push_back(path);
			this->scripts.push_back(filename);
		}
	}
}

int c_lua::get_script_id(std::string name) {
	for (int i = 0; i < this->scripts.size(); i++) {
		if (this->scripts.at(i) == name)
			return i;
	}

	return -1;
}

int c_lua::get_script_id_by_path(std::string path) {
	for (int i = 0; i < this->pathes.size(); i++) {
		if (this->pathes.at(i).string() == path)
			return i;
	}

	return -1;
}

std::string c_lua::get_script_path(std::string name) {
	return this->get_script_path(this->get_script_id(name));
}

std::string c_lua::get_script_path(int id) {
	if (id == -1)
		return  "";

	return this->pathes.at(id).string();
}
