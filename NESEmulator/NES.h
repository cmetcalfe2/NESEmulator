#pragma once
#include "CPU.h"
#include "PPU.h"
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

	void Cycle();

	CPU* processor;
	PPU* ppu;
	Memory* memory;
	Mapper* mapper;
	Interrupts* interrupts;

private:
	void SetMapper(uint8_t mapperNum);

};

