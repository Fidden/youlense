#include "includes.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI Hooks::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);
	}

	if (msg == WM_KEYDOWN)
	{
		if (wParam == g_config.i["misMenukey"])
			g_menu.set_menu_opened(!g_menu.is_menu_opened());

		if (wParam == g_config.i["_preset_0"])
		{
			g_config.i["_preset"] = 0;
			g_config.load();
		}

		if (wParam == g_config.i["_preset_1"])
		{
			g_config.i["_preset"] = 1;
			g_config.load();
		}

		if (wParam == g_config.i["_preset_2"])
		{
			g_config.i["_preset"] = 2;
			g_config.load();
		}

		if (wParam == g_config.i["_preset_3"])
		{
			g_config.i["_preset"] = 3;
			g_config.load();
		}

		if (wParam == g_config.i["_preset_4"])
		{
			g_config.i["_preset"] = 4;
			g_config.load();
		}

		if (wParam == g_config.i["_preset_5"])
		{
			g_config.i["_preset"] = 5;
			g_config.load();
		}
	}

	if (g_menu.is_menu_opened()) {
		ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);

		if (wParam != 'W' && wParam != 'A' && wParam != 'S' && wParam != 'D' && wParam != VK_SHIFT && wParam != VK_CONTROL && wParam != VK_TAB && wParam != VK_SPACE || ui::GetIO().WantTextInput)
			return true;
	}

	return CallWindowProcA(g_hooks.m_old_wndproc, hwnd, msg, wParam, lParam);
}