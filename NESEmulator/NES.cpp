#include "NES.h"



NES::NES()
{
	memory = new Memory();
	processor = new CPU(memory);
}


NES::~NES()
{
	delete processor;
	processor = NULL;

	delete memory;
	memory = NULL;
}


bool NES::LoadROM(std::string path)
{
	std::ifstream input(path, std::ios::binary | std::ios::ate);

	if (!input.fail())
	{
		unsigned int inputSize = input.tellg();
		inputSize = (inputSize > 0xFFFF) ? 0xFFFF : inputSize;
		std::cout << inputSize << std::endl;

		input.seekg(0, std::ios::beg);

		// Check if is iNES file
		char headerBuffer[4];
		headerBuffer[3] = '\0';
		for (int i = 0; i < 3; i++)
		{
			headerBuffer[i] = input.get();
		}
		uint8_t verificationByte = input.get();

		if (std::strcmp(headerBuffer, "NES") != 0 || verificationByte != 0x1A)
		{ 
			printf("Failed to load program (not iNES file) - %s\n", path.c_str());
			return false;
		}

		uint8_t prgBanks = input.get();
		uint8_t chrBanks = input.get();

		uint8_t controlByte1 = input.get();
		uint8_t controlByte2 = input.get();

		// Check control byte stuff here

		// End control byte checking

		input.seekg(16, std::ios::beg);
		for (unsigned int i = 0; i < 16384; i++)
		{
			// Start loading program to memory location 0x8000
			memory->SetByte(0x8000 + i, input.get());
		}

		input.seekg(16, std::ios::beg);
		for (unsigned int i = (prgBanks - 1) * 16384; i < 16384; i++)
		{
			// Start loading program to memory location 0xC000
			memory->SetByte(0xC000 + i, input.get());
		}

		processor->Reset();

		return true;
	}
	else
	{
		printf("Failed to load program - %s\n", path.c_str());
		return false;
	}
}

void NES::Cycle()
{
	processor->Cycle();
}
