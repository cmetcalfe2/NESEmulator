#pragma once
#include <SDL.h>
#include <cstdio>
#include "RenderTexture.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init();
	void CopyEmulatorFrameToTexture(void* pixels);
	void Render();

private:
	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int TEXTURE_WIDTH = 256;
	const int TEXTURE_HEIGHT = 240;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	// Texture to render emulator frames to
	RenderTexture* renderTexture = NULL;
};

