#include "CheatEngine.h"
#include "../DllMain/DllMain.h"


CheatEngine::CheatEngine(HINSTANCE hinstance)
{
	OutputDebugStringA_1Param("[GN]:%s", __FUNCTION__);
	ce = this;
	this->self_module_handle = hinstance;	
	//Set overlay window handle
	this->Draw::SetOverlayWindowHinstance(hinstance);
	//Find game windowhandle and set game windowhandle
	while (this->Draw::GetGameWindowHandle() == NULL)
	{	this->Draw::SetGameWindowHandle(this->CheatEngineApi::FindWindowA("CrossFire", "´©Ô½»ðÏß"));	}
	DWORD game_pid = 0;
	this->CheatEngineApi::GetWindowThreadProcessId(this->CheatEngine::GetGameWindowHandle(), &game_pid);
	this->CheatEngine::SetGameProcessId(game_pid);
	////Set my driver class
	//this->CheatEngine::Drv = new Driver(NULL, NULL);
	//this->CheatEngine::Drv->SetPID(this->SelfAPI::GetCurrentProcessId());
	//this->Game::GetDriver(this->CheatEngine::Drv);
	////Get game baseaddress
	//this->Game::BaseAddressInit();
	//Create Overlay Window
	if (!this->Draw::CreateOverlayWindow(L"GN_Window", L"GN_OverlayWindow", true/*inverse_screenshot*/))
		OutputDebugStringA_1Param("[GN]:%s-> CreateOverlayWindow() failed", __FUNCTION__);
	////Set Vector exception handler
	//veh = new Veh;
	////veh->Veh::SetVectoredExceptionHandler(VectoredHandler);
	//veh->Veh::SetUnhandledExceptionFilter(VectoredHandler);
	//int ret = veh->Veh::SetHardWareBreakPoint(L"crossfire.exe", 0x455,
	//	this->Game::GameBase.ACE_BASE64 + GlobalBaseFuncOffset,
	//	Hitchaddress,
	//	RedNameTrackAddress,
	//	SilentTrackAddress);
	////Set software break pointer
	//this->CheatEngine::InitSoftWareBreakPoint(veh);
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
			//this->Draw::MenuDraw();
			//this->Draw::MainFuncDraw();
			//this->Draw::EndDirect11();
						
			//you can also be in game window callback's WM_MOUSEMOVE message use this function too.
			//this->Draw::MoveOverlayWindow(this->Draw::GetGameWindowHandle(), this->Draw::GetOverlayWindowHandle());
			//this->Draw::GetGameWindowRect();
			this->Draw::BegineDirect11();
			this->Draw::SetImGuiMouse();
			this->Draw::MenuDraw();
			//this->Draw::MainFuncDraw();
			this->Draw::EndDirect11();
		}
	}
	//// Cleanup
	//ImGui_ImplDX11_Shutdown();
	//ImGui_ImplWin32_Shutdown();
	//ImGui::DestroyContext();
	//this->CleanupDeviceD3D11();
	//::DestroyWindow(this->GetOverlayWindowHandle());
}



