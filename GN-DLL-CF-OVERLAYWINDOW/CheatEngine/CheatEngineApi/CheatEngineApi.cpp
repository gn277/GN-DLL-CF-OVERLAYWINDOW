#include "CheatEngineApi.h"


CheatEngineApi::CheatEngineApi()
{
	//Load Modulehandle
	user32dll = LoadLibraryA("user32.dll");
	kernel32dll = LoadLibraryA("KERNEL32.dll");
	//Get api address
	if (user32dll != 0)
	{
		findwindowaaddress = (FindWindowAStruct)GetProcAddress(user32dll, "FindWindowA");
		getwindowthreadprocessid = (GetWindowThreadProcessIdStruct)GetProcAddress(user32dll, "GetWindowThreadProcessId");
	}
	else
	{
		OutputDebugStringA("[GN]:user32dll failed!");
		exit(0);
	}
	if (kernel32dll != 0)
	{
		getcurrentprocessid = (GetCurrentProcessIdStruct)GetProcAddress(kernel32dll, "GetCurrentProcessId");
		getcurrentprocess = (GetCurrentProcessStruct)GetProcAddress(kernel32dll, "GetCurrentProcess");
		getmodulehandlea = (GetModuleHandleAStruct)GetProcAddress(kernel32dll, "GetModuleHandleA");
	}
	else
	{
		OutputDebugStringA("[GN]:kernel32dll failed!");
		exit(0);
	}
}

CheatEngineApi::~CheatEngineApi()
{
}

HWND CheatEngineApi::FindWindowA(_In_opt_ LPCSTR lpClassName, _In_opt_ LPCSTR lpWindowName)
{
	try
	{
		return findwindowaaddress(lpClassName, lpWindowName);
	}
	catch (...)
	{
		OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	}
}

DWORD CheatEngineApi::GetWindowThreadProcessId(_In_ HWND hWnd, _Out_opt_ LPDWORD lpdwProcessId)
{
	try
	{
		return getwindowthreadprocessid(hWnd, lpdwProcessId);
	}
	catch (...)
	{
		OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	}
}

DWORD CheatEngineApi::GetCurrentProcessId()
{
	try
	{
		return getcurrentprocessid();
	}
	catch (...)
	{
		OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	}
}

HANDLE CheatEngineApi::GetCurrentProcess()
{
	try
	{
		return getcurrentprocess();
	}
	catch (...)
	{
		OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	}
}

HMODULE CheatEngineApi::GetModuleHandleA(_In_opt_ LPCSTR lpModuleName)
{
	try
	{
		return getmodulehandlea(lpModuleName);
	}
	catch (...)
	{
		OutputDebugStringA_1Param("[GN]:%s->", __FUNCTION__);
	}
}
