#pragma once
#include <SDL.h>
#include <cstdio>
#include <memory>

class RenderTexture
{
public:
	RenderTexture(int height, int width);
	~RenderTexture();

	bool CreateBlank(SDL_Renderer* renderer);

	bool LockTexture();
	void UnlockTexture();
	void CopyPixels(void* pixels);

	void RenderToScreen(SDL_Renderer* renderer);

	SDL_Texture* texture = NULL;

private:
	int width;
	int height;
	int pitch;

	void* texturePixels = NULL;
};

