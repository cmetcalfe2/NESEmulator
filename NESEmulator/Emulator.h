#pragma once
#include <SDL.h>

#include "NES.h"
#include "Renderer.h"
#include "InputManager.h"

class Emulator
{
public:
	Emulator();
	~Emulator();

	bool Init();
	bool Run();

private:
	void PollEvents();

	NES nes;
	Renderer* renderer;

	bool running = true;

	bool romLoaded = false;
	bool pauseEmulation = false;

};

