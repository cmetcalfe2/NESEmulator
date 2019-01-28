#include "InputManager.h"


InputManager* InputManager::instance;


InputManager::InputManager()
{
	LoadFromSettings();
	Settings::instance->AddSettingsLoadedListener(std::bind(&InputManager::LoadFromSettings, this));
}

InputManager::~InputManager()
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		delete connectedControllers[i];
		connectedControllers[i] = NULL;
	}

	for (int i = 0; i < physicalGamepads.size(); i++)
	{
		if (physicalGamepads[i] != NULL)
		{
			SDL_GameControllerClose(physicalGamepads[i]);
		}
	}
}

void InputManager::Init()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
	instance = new InputManager();
}

void InputManager::InitGamepads()
{
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		physicalGamepads.push_back(SDL_GameControllerOpen(i));
	}
}

void InputManager::HandleEvent(SDL_Event e)
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		if (connectedControllers[i] != NULL)
		{
			connectedControllers[i]->HandleEvent(e);
		}
	}
}

void InputManager::SetController(int index, Controller* controller)
{
	if (connectedControllers[index] != NULL)
	{
		delete connectedControllers[index];
		connectedControllers[index] = NULL;
	}

	connectedControllers[index] = controller;
}

uint8_t InputManager::ReadController(int controller)
{
	if (connectedControllers[controller] != NULL)
	{
		return connectedControllers[controller]->Read();
	}
	else
	{
		return 0x02; // Controller not connected
	}
}

void InputManager::WriteController(int controller, uint8_t val)
{
	if (connectedControllers[controller] != NULL)
	{
		connectedControllers[controller]->Write(val);
	}
}

void InputManager::LoadFromSettings()
{
	for (int i = 0; i < MAX_CONTROLLERS; i++)
	{
		delete connectedControllers[i];
		connectedControllers[i] = NULL;

		ControllerSettings controllerSettings = Settings::instance->controllerSettings[i];

		if (controllerSettings.controllerProfileIndex != -1)
		{
			ControllerProfile controllerProfile = Settings::instance->controllerProfiles[controllerSettings.controllerProfileIndex];

			switch (controllerProfile.nesControllerType)
			{
			case NESCONTROLLER_STANDARD:
				connectedControllers[i] = new StandardController();
				connectedControllers[i]->LoadProfile(controllerProfile);
				connectedControllers[i]->SetPhysicalController(controllerSettings.physicalControllerIndex);
				break;
			}
		}
	}
}
