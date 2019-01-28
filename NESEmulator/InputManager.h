#pragma once
#include "Controller.h"
#include "StandardController.h"
#include "Settings.h"
#include <stdint.h>
#include "SDL.h"

class InputManager
{
public:
	InputManager();
	~InputManager();

	static void Init();
	static InputManager* GetInstance() { return instance; }

	void InitGamepads();

	void HandleEvent(SDL_Event e);

	void SetController(int index, Controller* controller);

	uint8_t ReadController(int controller);
	void WriteController(int controller, uint8_t val);

	void LoadFromSettings();

private:
	static InputManager* instance;

	Controller* connectedControllers[MAX_CONTROLLERS];
	
	std::vector<SDL_GameController*> physicalGamepads;
};

