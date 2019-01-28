#pragma once
#include <stdint.h>
#include "Settings.h"
#include "SDL.h"

class Controller
{
public:
	Controller();
	~Controller();

	void LoadProfile(ControllerProfile profile);
	void SetPhysicalController(int index) { physicalControllerIndex = index; }
	virtual void HandleEvent(SDL_Event e) = 0;

	virtual uint8_t Read() = 0;
	virtual void Write(uint8_t val) = 0;

protected:
	ControllerProfile profile;

	int physicalControllerIndex = 0;
};

