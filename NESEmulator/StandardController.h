#pragma once
#include "Controller.h"
#include "Settings.h"

class StandardController :
	public Controller
{
public:
	StandardController();
	~StandardController();

	void HandleEvent(SDL_Event e) override;

	uint8_t Read() override;
	void Write(uint8_t val) override;

	std::map<NESInputs, uint8_t> buttonsPressed;

private:
	bool shiftingDisabled = false;
	int curButtonIndex = 0;

	std::vector<NESInputs> buttonReadingOrder = {
		BUT_A,
		BUT_B,
		BUT_SELECT,
		BUT_START,
		DPAD_U,
		DPAD_D,
		DPAD_L,
		DPAD_R
	};
};

