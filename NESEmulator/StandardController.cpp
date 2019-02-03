#include "StandardController.h"



StandardController::StandardController()
{
	for (auto &it : Settings::controllerButtons[NESCONTROLLER_STANDARD])
	{
		buttonsPressed[it] = false;
	}
}


StandardController::~StandardController()
{
}

void StandardController::HandleEvent(SDL_Event e)
{
	if ((e.type == SDL_CONTROLLERBUTTONDOWN || e.type == SDL_CONTROLLERBUTTONUP) && profile.inputControllerType == INPUTTYPE_CONTROLLER && e.cbutton.which == physicalControllerIndex)
	{
		if (e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			for (auto &it : profile.bindings)
			{
				if (it.second == e.cbutton.button)
				{
					buttonsPressed[it.first] = 1;
					break;
				}
			}
		}
		else
		{
			for (auto &it : profile.bindings)
			{
				if (it.second == e.cbutton.button)
				{
					buttonsPressed[it.first] = 0;
					break;
				}
			}
		}
	}
	else if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && profile.inputControllerType == INPUTTYPE_KEYBOARD)
	{
		if (e.type == SDL_KEYDOWN)
		{
			for (auto &it : profile.bindings)
			{
				if (it.second == e.key.keysym.sym)
				{
					buttonsPressed[it.first] = 1;
					break;
				}
			}
		}
		else
		{
			for (auto &it : profile.bindings)
			{
				if (it.second == e.key.keysym.sym)
				{
					buttonsPressed[it.first] = 0;
					break;
				}
			}
		}
	}
}

uint8_t StandardController::Read()
{
	if (curButtonIndex < 8)
	{
		uint8_t val = 0x40; // Open bus returns high byte of controller address in bits 5-7
		val |= buttonsPressed[buttonReadingOrder[curButtonIndex]];

		if (!shiftingDisabled)
		{
			curButtonIndex++;
		}

		return val;
	}
	else
	{
		return 0x40;
	}
}

void StandardController::Write(uint8_t val)
{
	if (val & 1)
	{
		shiftingDisabled = true;
		curButtonIndex = 0;
	}
	else
	{
		shiftingDisabled = false;
	}
}
