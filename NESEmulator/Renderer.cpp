#include "Renderer.h"



Renderer::Renderer(NES* n)
{
	nes = n;
}


Renderer::~Renderer()
{
	// Destroy texture
	delete renderTexture;
	renderTexture = NULL;

	// Destroy renderer
	SDL_DestroyRenderer(renderer);

	// Destroy window
	SDL_DestroyWindow(window);

	// Destroy imgui context
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
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


	// Init imgui
	ImGui::CreateContext();
	ImGuiSDL::Initialize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
	ImGui_ImplSDL2_InitForOpenGL(window, nullptr);

	return true;
}

void Renderer::CopyEmulatorFrameToTexture(void* pixels)
{
	renderTexture->LockTexture();
	renderTexture->CopyPixels(pixels);
	renderTexture->UnlockTexture();
}

void Renderer::RenderUI()
{
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	RenderMainMenuBar();
	RenderFileDialog();
	RenderControllerSetupDialog();
	RenderControllerProfileDialog();
	RenderEditControllerProfileDialog();
	//ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGuiSDL::Render(ImGui::GetDrawData());
}

void Renderer::Render()
{
	renderTexture->RenderToScreen(renderer);
	RenderUI();
	SDL_RenderPresent(renderer);
}
