#include "includes.h"

HRESULT WINAPI Hooks::Present(IDirect3DDevice9* pDevice, RECT* pRect1, const RECT* pRect2, HWND hWnd, const RGNDATA* pRGNData) 
{
	g_visuals.m_thirdperson = g_config.auto_check("effects_thirdperson_key");
	IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state);
	pDevice->GetVertexDeclaration(&vertDec);
	pDevice->GetVertexShader(&vertShader);

	static auto wanted_ret_address = _ReturnAddress();
	if (_ReturnAddress() == wanted_ret_address)
	{
		DWORD colorwrite, srgbwrite;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		static bool once{ false };
		if (!once) {
			g_menu.initialize(pDevice);

			g_config.load_defaults();
			g_config.load_keys();
			once = true;
		}

		g_menu.draw_begin();
		{
			g_menu.draw();
			g_visuals.AutoPeekDraw(ui::GetDrawList());
		}
		g_menu.draw_end();

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
	}

	pixel_state->Apply();
	pixel_state->Release();
	pDevice->SetVertexDeclaration(vertDec);
	pDevice->SetVertexShader(vertShader);

	return g_hooks.m_device.GetOldMethod<decltype(&Present)>(17)(pDevice, pRect1, pRect2, hWnd, pRGNData);
}

HRESULT WINAPI Hooks::Reset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentParameters)
{
	if (!g_menu.is_menu_initialized())
		return g_hooks.m_device.GetOldMethod<decltype(&Reset)>(16)(pDevice, pPresentParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();
	auto result = g_hooks.m_device.GetOldMethod<decltype(&Reset)>(16)(pDevice, pPresentParameters);
	ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}