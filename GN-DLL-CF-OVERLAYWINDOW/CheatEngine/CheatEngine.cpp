#include "CheatEngine.h"
#include "../DllMain/DllMain.h"


CheatEngine::CheatEngine(HINSTANCE hinstance)
{
	OutputDebugStringA_1Param("[GN]:%s", __FUNCTION__);

	//Save Modulehandle
	this->self_module_handle = hinstance;

	ce = this;

	//Set overlay window handle
	this->Draw::SetOverlayWindowHinstance(hinstance);

	//Find game windowhandle and set game windowhandle
	while (this->Draw::GetGameWindowHandle() == NULL)
	{	this->Draw::SetGameWindowHandle(this->CheatEngineApi::FindWindowA("CrossFire", "穿越火线"));	}

	DWORD game_pid = 0;
	this->CheatEngineApi::GetWindowThreadProcessId(this->CheatEngine::GetGameWindowHandle(), &game_pid);
	this->CheatEngine::SetGameProcessId(game_pid);

	////Set my driver class
	this->CheatEngine::driver = new Driver;
	this->CheatEngine::driver->SetProcessID(this->CheatEngineApi::GetCurrentProcessId());
	
	//Get game baseaddress
	this->Game::BaseAddressInit();

	//Create Overlay Window
	if (!this->Draw::CreateOverlayWindow(L"GN_Window", L"GN_OverlayWindow", true/*inverse_screenshot*/))
		OutputDebugStringA_1Param("[GN]:%s-> CreateOverlayWindow() failed", __FUNCTION__);

	//Set Exception Handler
	if (!gn_exception->InstallException(CheatEngine::NewExceptionHandler))
		exit(0);
	int ret = gn_exception->GN_Exception::SetHardWareBreakPoint(L"crossfire.exe", 0x455,
	/*0*/this->Game::GameBase.ACE_BASE64 + GlobalBaseFuncOffset,
	/*0*/Hitchaddress,
	/*0*/RedNameTrackAddress,
	0/*SilentTrackAddress*/);//视角追踪需要更新，不能用

	////Clear Modulehandle Header
	//RtlZeroMemory(this->self_module_handle, 1024);
	//Hide Dll Memory
	if (!this->CheatEngine::driver->HideMemoryByVAD((ULONG64)this->self_module_handle, 0/*模块大小*/))
	{
		OutputDebugStringA("[GN]:HideMemoryByVAD() error!");
		exit(0);
	}

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
	if (caller_address == ce->GameBase.Cshell + 0xA30D70)
	{
		OutputDebugStringA_1Param("[GN]:开枪，call地址：%p", caller_address);
		return true;
	}

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

