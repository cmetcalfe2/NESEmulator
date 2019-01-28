#pragma once
#include <SDL.h>
#include <cstdio>
#include <string>
#include "NES.h"
#include "RenderTexture.h"
#include "imgui/imgui.h"
#include "imgui_sdl.h"
#include "imgui_impl_sdl.h"
#include "ImGuiFileDialog.h"
#include "imgui_stdlib.h"
#include "Settings.h"

class Renderer
{
public:
	Renderer(NES* nes);
	~Renderer();

	bool Init();
	void CopyEmulatorFrameToTexture(void* pixels);
	void Render();

private:
	void RenderUI();

	// UI
	void RenderMainMenuBar();
	void RenderFileDialog();
	void RenderControllerSetupDialog();
	void RenderControllerProfileDialog();
	void RenderEditControllerProfileDialog();

	std::string GetButtonName(int button);
	void BindButton(NESInputs button);
	int SetPhysicalController();

	bool showOpenROMFileDialog = false;
	bool showControllerProfilesDialog = false;

	bool showControllerSettingsDialog = false;

	bool showNewControllerProfileDialog = false;
	ControllerProfile curControllerProfile;

	bool showEditControllerProfileDialog = false;
	int editControllerProfileIndex = 0;

	const int SCREEN_WIDTH = 800;
	const int SCREEN_HEIGHT = 600;
	const int TEXTURE_WIDTH = 256;
	const int TEXTURE_HEIGHT = 240;

	NES* nes;

	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;

	// Texture to render emulator frames to
	RenderTexture* renderTexture = NULL;
};

