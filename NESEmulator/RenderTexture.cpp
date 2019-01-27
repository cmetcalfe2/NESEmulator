#include "RenderTexture.h"



RenderTexture::RenderTexture(int w, int h)
{
	width = w;
	height = h;
}


RenderTexture::~RenderTexture()
{
	SDL_DestroyTexture(texture);
	texture = NULL;
	texturePixels = NULL;
}


bool RenderTexture::CreateBlank(SDL_Renderer* renderer)
{
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (texture == NULL)
	{
		printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
		return false;
	}
}

bool RenderTexture::LockTexture()
{
	if (SDL_LockTexture(texture, NULL, &texturePixels, &pitch) != 0)
	{
		printf("Unable to lock texture! %s\n", SDL_GetError());
		return false;
	}
	else
	{
		return true;
	}
}

void RenderTexture::UnlockTexture()
{
	SDL_UnlockTexture(texture);
	texturePixels = NULL;
	pitch = 0;
}

void RenderTexture::CopyPixels(void* pixels)
{
	if (texturePixels != NULL)
	{
		memcpy(texturePixels, pixels, pitch * height);
	}
}

void RenderTexture::RenderToScreen(SDL_Renderer* renderer)
{
	SDL_RenderCopy(renderer, texture, NULL, NULL);
}
