#include "Emulator.h"



Emulator::Emulator()
{
}


Emulator::~Emulator()
{
}

bool Emulator::Init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		if (!renderer.Init())
		{
			return false;
		}
	}

	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/instr_test-v5/official_only.nes");
	romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/ROM Collection/Balloon Fight (USA).nes");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/Zelda.NES");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/nestest.nes");
	
	return true;
}

bool Emulator::Run()
{
	PollEvents();

	if (romLoaded && !pauseEmulation)
	{
		nes.RunOneFrame();
		renderer.CopyEmulatorFrameToTexture(nes.ppu->GetPixelBuffer());
		renderer.Render();
	}

	return running;
}

void Emulator::PollEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		//User requests quit
		if (e.type == SDL_QUIT)
		{
			running = false;
		}
		else if (e.type == SDL_KEYDOWN)
		{
			if (e.key.keysym.sym == SDLK_BACKSPACE)
			{
				nes.ppu->DumpVRAM();
			}
		}
	}
}
