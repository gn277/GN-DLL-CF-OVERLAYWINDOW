#pragma once
#include "../../pch.h"
#include "DataStruct.h"


class Game
{
private:
	HANDLE m_handle = 0;									//����dll���
	HWND game_window_handle = 0;							//��Ϸ���ھ��
	DWORD game_processid = 0;								//����PID
	WNDPROC original_proc = nullptr;						//ԭʼ���ڻص����

public:

public:

public:
	Game();
	~Game();

};


