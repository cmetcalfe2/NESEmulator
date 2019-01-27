#pragma once
#include <SDL.h>

#include "NES.h"
#include "Renderer.h"

class Emulator
{
public:
	Emulator();
	~Emulator();

	bool Init();
	bool Run();

private:
	bool running = true;

	bool romLoaded = false;
	bool pauseEmulation = false;

	NES nes;
	Renderer renderer;

	void PollEvents();
};

