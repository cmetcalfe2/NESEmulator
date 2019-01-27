#include "Emulator.h"

int main(int argc, char* argv[])
{
	/*NES nes;
	bool romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/instr_test-v5/official_only.nes");

	if (romLoaded)
	{
		while (true)
		{
			nes.Cycle();
		}
	}

	std::string test;
	std::cin >> test;*/

	//CPUTestBlargg test;
	//test.Start();

	/*SDL_Window* window = NULL;

	//The surface contained by the window
	SDL_Surface* screenSurface = NULL;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
	}
	else
	{
		//Create window
		window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);

			//Fill the surface white
			SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0x00, 0x00));

			//Update the surface
			SDL_UpdateWindowSurface(window);

			//Wait two seconds
			SDL_Delay(2000);
		}
	}*/

	Emulator emulator;

	bool initSuccess = emulator.Init();

	if (!initSuccess)
	{
		SDL_Quit();
		return 0;
	}

	bool running = true;

	while (running)
	{
		running = emulator.Run();
	}

	SDL_Quit();

	return 0;
}