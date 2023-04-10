#pragma once
#include "../pch.h"
#include "Draw/Draw.h"
#include "Game/Game.h"
#include "Tools/Tools.h"
#include "MemoryTools/MemoryTools.h"
#include "CheatEngineApi/CheatEngineApi.h"


class CheatEngine :public Draw, public Game, public MemoryTools, public Tools,public CheatEngineApi
{
private:
	HMODULE self_module_handle = NULL;

public:

public:
	CheatEngine(HINSTANCE hinstance);
	~CheatEngine();
	void Rendering();

};


