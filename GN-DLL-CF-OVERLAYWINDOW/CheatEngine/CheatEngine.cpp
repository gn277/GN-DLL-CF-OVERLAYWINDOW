#include "CheatEngine.h"


CheatEngine::CheatEngine()
{

}

CheatEngine::~CheatEngine()
{

}

void CheatEngine::Rendering()
{
	MSG Message = { NULL };
	while (Message.message != WM_QUIT)
	{
		::GetMessageA(&Message, NULL, NULL, NULL);
		if (Message.message == WM_QUIT)
			break;
		//Window message loop
		if (PeekMessage(&Message, this->Draw::GetOverlayWindowHandle(), 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		}
		else
		{
			////you can also be in game window callback's WM_MOUSEMOVE message use this function too.
			////this->Draw::MoveOverlayWindow(this->CheatEngine::game_window_handle, this->Draw::GetOverlayWindowHandle());
			////this->Draw::GetGameWindowRect();
			//this->Draw::BegineDirect11();
			////this->Draw::SetImGuiMouse();
			//this->CheatEngine::MenuDraw();
			//this->CheatEngine::MainFuncDraw();
			//this->Draw::EndDirect11();
		}
	}
	//// Cleanup
	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();
	//this->CleanupDeviceD3D11();
	//::DestroyWindow(this->GetOverlayWindowHandle());
}



