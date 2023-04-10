#pragma once
#include "../../pch.h"
#include "DataStruct.h"


class Game
{
private:
	HANDLE m_handle = 0;									//自身dll句柄
	HWND game_window_handle = 0;							//游戏窗口句柄
	DWORD game_processid = 0;								//进程PID
	WNDPROC original_proc = nullptr;						//原始窗口回调句柄

public:

public:

public:
	Game();
	~Game();

};


