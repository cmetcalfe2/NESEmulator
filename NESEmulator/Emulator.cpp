#include "Emulator.h"



Emulator::Emulator()
{
	renderer = new Renderer(&nes);
}


Emulator::~Emulator()
{
	delete renderer;
	renderer = NULL;
}

bool Emulator::Init()
{
	// Load settings
	Settings::Init();

	// Create input manager
	InputManager::Init();

	//Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		if (!renderer->Init())
		{
			return false;
		}
		else
		{
			InputManager::GetInstance()->InitGamepads();
		}
	}

	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/instr_test-v5/rom_singles/15-brk.nes");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/ROM Collection/Balloon Fight (USA).nes");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/Zelda.NES");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/nestest.nes");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/Super_Mario_Bros..nes");
	//romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/nes-test-roms-master/sprite_overflow_tests/1.Basics.nes");
	
	return true;
}

bool Emulator::Run()
{
	PollEvents();

	if (nes.romLoaded && !pauseEmulation)
	{
		nes.RunOneFrame();
		renderer->CopyEmulatorFrameToTexture(nes.ppu->GetPixelBuffer());
	}

	renderer->Render();

	return running;
}

void Emulator::PollEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		// UI Event handling
		ImGui_ImplSDL2_ProcessEvent(&e);

		// Controller input
		InputManager::GetInstance()->HandleEvent(e);

		//User requests quit
		if (e.type == SDL_QUIT)
		{
			running = false;
		}
	}
}
