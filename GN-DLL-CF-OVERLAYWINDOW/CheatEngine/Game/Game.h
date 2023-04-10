#pragma once
#include "../../pch.h"
#include "DataStruct.h"


class Game
{
private:
	DWORD game_processid = 0;								//进程PID
	WNDPROC original_proc = nullptr;						//原始游戏窗口回调句柄

public:
	Game();
	~Game();

};


