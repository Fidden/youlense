#pragma once
#include "includes.h"
#pragma comment(lib, "freetype.lib")

class Menu {
public:
	void initialize(IDirect3DDevice9* pDevice);
	void draw_begin();
	void draw();
	void draw_end();

	bool is_menu_initialized();
	bool is_menu_opened();
	void set_menu_opened(bool v);

	IDirect3DTexture9* get_texture_data();
	ImColor get_accent_color();

private:
	bool m_bInitialized;
	bool m_bIsOpened;
	IDirect3DTexture9* m_pTexture;

	int m_nCurrentTab;
};

extern Menu g_menu;