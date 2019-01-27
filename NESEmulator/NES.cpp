#include "NES.h"



NES::NES()
{
	memory = new Memory();
	interrupts = new Interrupts();
	processor = new CPU(memory, interrupts);
	ppu = new PPU(memory, interrupts);

	memory->SetPPU(ppu);
}


NES::~NES()
{
	delete processor;
	processor = NULL;

	delete ppu;
	ppu = NULL;

	delete memory;
	memory = NULL;

	if (mapper != NULL)
	{
		delete mapper;
		mapper = NULL;
	}

	delete interrupts;
	interrupts = NULL;
}


bool NES::LoadROM(std::string path)
{
	std::ifstream input(path, std::ios::binary | std::ios::ate);

	if (!input.fail())
	{
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

		uint8_t numPRGBanks = input.get();
		uint8_t numCHRBanks = input.get();

		uint8_t controlByte1 = input.get();
		uint8_t controlByte2 = input.get();

		// Mirroring mode
		NametableMirroringMode mirroringMode = MIRRORING_SINGLESCREEN;
		if (controlByte1 & 1)
		{
			mirroringMode = MIRRORING_VERTICAL;
		}
		else
		{
			mirroringMode = MIRRORING_HORIZONTAL;
		}

		if (controlByte1 & 8)
		{
			mirroringMode = MIRRORING_4SCREEN;
		}

		ppu->SetMirrorMode(mirroringMode);

		// Mapper
		uint8_t mapperNumber = (controlByte1 & 0x0F) + ((controlByte2 & 0x0F) << 4);
		SetMapper(mapperNumber);

		// Check control byte stuff here

		// End control byte checking

		// Load PRG ROM
		input.seekg(16, std::ios::beg);
		mapper->LoadPRGROM(input, numPRGBanks);
		mapper->LoadCHRROM(input, numCHRBanks);

		processor->OnReset();

		return true;
	}
	else
	{
		printf("Failed to load program - %s\n", path.c_str());
		return false;
	}
}

void NES::SetMapper(uint8_t mapperNumber)
{
	switch (mapperNumber)
	{
	case 0:
		mapper = new MapperNROM(ppu);
		break;
	case 1:
		mapper = new MapperMMC1(ppu);
		break;
	default:
		printf("Unsupported mapper - %i!", mapperNumber);
		break;
	}

	processor->SetMapper(mapper);
	ppu->SetMapper(mapper);
	memory->SetMapper(mapper);
}

void NES::Cycle()
{
	processor->Cycle();
	for (int i = 0; i < ppuCyclesPerCPUCycle; i++)
	{
		ppu->Cycle();
	}
	interrupts->PollInterruptLines();
}

void NES::RunOneFrame()
{
	uint32_t cpuCycles = 0;
	while (cpuCycles < cpuCyclesPerFrame)
	{
		Cycle();
		cpuCycles++;
	}
}
