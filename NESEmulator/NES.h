#pragma once
#include "CPU.h"
#include "PPU.h"
#include "APU.h"
#include "Memory.h"
#include "AllMappers.h"
#include <string>
#include <fstream>
#include <iostream>

class NES
{
public:
	NES();
	~NES();

	bool LoadROM(std::string path);

	void RunOneFrame();
	void Cycle();

	bool romLoaded = false;

	CPU* processor;
	PPU* ppu;
	APU* apu;
	Memory* memory;
	Mapper* mapper;
	InputManager* inputManager;
	Interrupts* interrupts;

private:
	void SetMapper(uint8_t mapperNum);

	const uint32_t cpuCyclesPerFrame = 1789773 / 60;
	const int ppuCyclesPerCPUCycle = 3;

	clock_t startTime;
	int elapsedFrames = 0;

};

