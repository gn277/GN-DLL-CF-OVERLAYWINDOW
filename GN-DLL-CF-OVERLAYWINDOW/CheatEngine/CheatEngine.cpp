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
	{	this->Draw::SetGameWindowHandle(this->CheatEngineApi::FindWindowA("CrossFire", "穿越火线"));	}
	DWORD game_pid = 0;
	this->CheatEngineApi::GetWindowThreadProcessId(this->CheatEngine::GetGameWindowHandle(), &game_pid);
	this->CheatEngine::SetGameProcessId(game_pid);
	////Set my driver class
	//this->CheatEngine::Drv = new Driver(NULL, NULL);
	//this->CheatEngine::Drv->SetPID(this->SelfAPI::GetCurrentProcessId());
	//this->Game::GetDriver(this->CheatEngine::Drv);
	//Get game baseaddress
	this->Game::BaseAddressInit();
	//Create Overlay Window
	if (!this->Draw::CreateOverlayWindow(L"GN_Window", L"GN_OverlayWindow", true/*inverse_screenshot*/))
		OutputDebugStringA_1Param("[GN]:%s-> CreateOverlayWindow() failed", __FUNCTION__);
	//Set Vector exception handler
	m_exception = new GN_Exception;
	//m_exception->GN_Exception::SetVectoredExceptionHandler(ExceptionHandler);
	m_exception->GN_Exception::SetUnhandledExceptionFilter(ExceptionHandler);
	int ret = m_exception->GN_Exception::SetHardWareBreakPoint(L"crossfire.exe", 0x455,
		0/*this->Game::GameBase.ACE_BASE64 + GlobalBaseFuncOffset*/,
		Hitchaddress,
		0/*RedNameTrackAddress*/,
		0/*SilentTrackAddress*/);//视角追踪需要更新，不能用
	////Set software break pointer
	//this->CheatEngine::InitSoftWareBreakPoint(veh);
}

CheatEngine::~CheatEngine()
{

}

void CheatEngine::Rendering()
{
	MSG Message = { NULL };
	while (true)
	{
		//Window message loop
		if (PeekMessage(&Message, this->Draw::GetOverlayWindowHandle(), 0, 0, PM_REMOVE))
		{
			DispatchMessage(&Message);
			TranslateMessage(&Message);
		}
		else
		{
			//you can also be in game window callback's WM_MOUSEMOVE message use this function too.
			//this->Draw::MoveOverlayWindow(this->CheatEngine::game_window_handle, this->Draw::GetOverlayWindowHandle());
			//this->Draw::GetGameWindowRect();
			this->Draw::SetImGuiMouse();
			this->Draw::BegineDirect11();
			this->Draw::MenuDraw();
			this->Draw::MainFuncDraw();
			this->Draw::EndDirect11();

			////you can also be in game window callback's WM_MOUSEMOVE message use this function too.
			//this->Draw::MoveOverlayWindow(this->Draw::GetGameWindowHandle(), this->Draw::GetOverlayWindowHandle());
			//this->Draw::GetGameWindowRect();
			//this->Draw::BegineDirect11();
			//this->Draw::SetImGuiMouse();
			//this->Draw::MenuDraw();
			////this->Draw::MainFuncDraw();
			//this->Draw::EndDirect11();
		}
	}
	// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	this->CleanupDeviceD3D11();
	::DestroyWindow(this->GetOverlayWindowHandle());
}

bool CheatEngine::ByPassCheck(PCONTEXT context)
{
	DWORD64 caller_address = ce->MemoryTools::ReadLong(context->Rsi);
	////绘制检测
	if ((caller_address > ce->Game::GameBase.Win32U) && (caller_address < ce->Game::GameBase.Win32UEnd))
		return true;
	if ((caller_address > ce->Game::GameBase.Gdi32) && (caller_address < ce->Game::GameBase.Gdi32End))
		return true;
	if ((caller_address > ce->Game::GameBase.D3D9) && (caller_address < ce->Game::GameBase.D3D9End))
		return true;
	//#if _REDBULLBRELEASE || _REDBULLBDEBUG
	//	////功能检测
	//	//DWORD64 callto_address = ce->MemoryTools::ReadLong(context->Rbx + 0x20);
	//	//if (caller_address == ce->CheatEngine::Game::GameBase.Cshell + 0x12C6890)
	//	//	return false;
	//	if ((caller_address > ce->Game::GameBase.Cross) && (caller_address < ce->Game::GameBase.CrossEndAddress))
	//		return true;
	//#endif
	return false;
}

