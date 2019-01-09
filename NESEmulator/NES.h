#pragma once
#include "CPU.h"
#include "Memory.h"
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

private:
	CPU* processor;
	Memory* memory;
	
};

