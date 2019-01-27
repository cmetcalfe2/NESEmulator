#include "Renderer.h"



Renderer::Renderer()
{
}


Renderer::~Renderer()
{
	// Destroy window
	SDL_DestroyWindow(window);

	// Destroy texture
	delete renderTexture;
	renderTexture = NULL;
}

bool Renderer::Init()
{
	//Create window
	window = SDL_CreateWindow("NES Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		// Make window resizable
		SDL_SetWindowResizable(window, SDL_TRUE);

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (renderer == NULL)
		{
			printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
			return false;
		}
		else
		{
			//Update the surface
			SDL_UpdateWindowSurface(window);

			//Create uninitialized texture
			renderTexture = new RenderTexture(TEXTURE_WIDTH, TEXTURE_HEIGHT);
			if (!renderTexture->CreateBlank(renderer))
			{
				return false;
			}

		}
	}
	return true;
}

void Renderer::CopyEmulatorFrameToTexture(void* pixels)
{
	renderTexture->LockTexture();
	renderTexture->CopyPixels(pixels);
	renderTexture->UnlockTexture();
}

void Renderer::Render()
{
	renderTexture->RenderToScreen(renderer);
	SDL_RenderPresent(renderer);
}
