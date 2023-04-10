#include "../../pch.h"
#include "../../DllMain/DllMain.h"


//Overlay window proc
LRESULT CALLBACK Draw::OverlayWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	//	return true;
	switch (msg)
	{
	case WM_SIZE:
	{
		if (ce->Draw::GetD3D11Device() != NULL && wParam != SIZE_MINIMIZED)
		{
			ce->Draw::CleanupRenderTarget();
			ce->Draw::GetSwapChain()->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			ce->Draw::CreateRenderTarget();
		}
		break;
	}
	case WM_SYSCOMMAND:
	{
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	}
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


