#pragma once

class Config {
public:
	std::unordered_map<std::string, bool> b;
	std::unordered_map<std::string, int> i;
	std::unordered_map<std::string, float> f;
	std::unordered_map<std::string, float[4]> c;
	std::unordered_map<std::string, std::unordered_map<int, bool>> m;

	std::vector<const char*> presets = { "Rage", "Legit", "HvH", "Body-aim", "Secret", "Headshot", "Test" };

	void init();
	void save();
	void save_keys();
	bool is_key_down(int key);
	bool is_key_up(int key);
	bool is_key_pressed(int key);
	void load();
	void load_defaults();
	void load_keys();
	bool auto_check(std::string key);
	
	Color imcolor_to_ccolor(float* col) {
		return Color((int)(col[0] * 255.f), (int)(col[1] * 255.f), (int)(col[2] * 255.f), (int)(col[3] * 255.f));
	}

	Color imcolor_to_ccolor(float* col, float alpha_override) {
		return Color((int)(col[0] * 255.f), (int)(col[1] * 255.f), (int)(col[2] * 255.f), (int)(alpha_override * 255.f));
	}
};

extern Config g_config;