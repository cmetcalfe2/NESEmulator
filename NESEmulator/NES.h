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

	void LoadROM(std::string path);

private:
	CPU* processor;
	Memory* memory;
	
};

