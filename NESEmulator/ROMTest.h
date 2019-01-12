#pragma once
#include <string>
#include "NES.h"

class ROMTest
{
public:
	ROMTest();
	~ROMTest();
	
	void Start();

protected:
	virtual void PostCycle() {};

	std::string romPath;

	NES nes;

	bool finishExecuting = false;
};

