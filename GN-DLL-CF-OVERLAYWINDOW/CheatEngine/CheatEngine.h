#pragma once
#include "../pch.h"
#include "Draw/Draw.h"
#include "Game/Game.h"
#include "Tools/Tools.h"
#include "MemoryTools/MemoryTools.h"
#include "CheatEngineApi/CheatEngineApi.h"
#include "../Driver/Driver.h"
#pragma comment(lib,"Driver/GN-Driver-Lib.lib")


class CheatEngine :public Game, public Draw, public MemoryTools, public Tools, public CheatEngineApi
{
private:
	HMODULE self_module_handle = NULL;

public:
	Driver* driver = nullptr;

public:
	void GameKeyCallBack(WPARAM wParam, LPARAM lParam);
	void GameCombinationKeyCallBack(WPARAM wParam, LPARAM lParam);
	static HRESULT Self_Present(IDirect3DDevice9* direct3ddevice9, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	static LONG WINAPI ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo);
	static LONG WINAPI NewExceptionHandler(PEXCEPTION_RECORD ExceptionRecord, PCONTEXT context);
	static LRESULT CALLBACK SelfGameWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	bool ByPassCheck(PCONTEXT context);

public:
	CheatEngine(HINSTANCE hinstance);
	~CheatEngine();
	void Rendering();

};


