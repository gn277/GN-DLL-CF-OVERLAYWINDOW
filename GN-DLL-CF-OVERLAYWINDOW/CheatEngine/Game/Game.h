#pragma once
#include "../../pch.h"
#include "DataStruct.h"


class Game
{
private:
	DWORD game_processid = 0;								//����PID
	WNDPROC original_proc = nullptr;						//ԭʼ��Ϸ���ڻص����

public:
	Game();
	~Game();

};


