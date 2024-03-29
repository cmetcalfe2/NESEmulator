#include "Memory.h"
#include "PPU.h"
#include "APU.h"
#include <stdint.h>


Memory::Memory()
{
	Clear();
	InitMemoryMap();
}


Memory::~Memory()
{
}

void Memory::Clear()
{
	for (int i = 0; i < MEMORY_AMOUNT; i++)
	{
		memory[i] = 0;
	}
}

void Memory::InitMemoryMap()
{
	for (uint32_t i = 0x0000; i < MEMORY_AMOUNT; i++)
	{
		memoryMap[i] = i;
	}

	// System memory mirroring
	uint16_t systemMirroringLocations[3] = { 0x0800, 0x1000, 0x1800 };
	for (int i = 0; i < 3; i++)
	{
		for (uint16_t j = 0x0000; j <= 0x07FF; j++)
		{
			memoryMap[systemMirroringLocations[i] + j] = j;
		}
	}

	// PPU I/O register mirroring
	for (uint16_t i = 0x2008; i <= 0x3FFF; i++)
	{
		memoryMap[i] = 0x2000 + (i % 8);
	}
}

uint8_t Memory::ReadByte(uint16_t addr)
{
	uint16_t realAddress = memoryMap[addr];

	if ((realAddress >= 0x2000 && realAddress <= 0x2007) || (realAddress >= 0x4000 && realAddress <= 0x401F))
	{
		// IO register
		return ReadIORegister(addr);
	}	
	else if (realAddress >= 0x8000)
	{
		// Read from PRG ROM/RAM
		return mapper->ReadPRGByte(realAddress);
	}
	else
	{
		return memory[realAddress];
	}
}

uint8_t Memory::ReadIORegister(uint16_t addr)
{
	switch (addr)
	{
	case 0x2002:
		// PPUSTATUS
		return ppu->ReadPPUSTATUS();
		break;
	case 0x2004:
		// OAMDATA
		return ppu->ReadOAMDATA();
		break;
	case 0x2007:
		// PPUDATA
		return ppu->ReadPPUDATA();
		break;
	case 0x4016:
		return InputManager::GetInstance()->ReadController(0);
		break;
	case 0x4017:
		return InputManager::GetInstance()->ReadController(1);
		break;
	default:
		return 0x0000;
		break;
	}
}

uint16_t Memory::ReadWord(uint16_t addr)
{
	return ReadByte(addr) + (uint16_t(ReadByte(addr + 1)) << 8);
}

void Memory::SetByte(uint16_t addr, uint8_t val)
{
	uint16_t realAddress = memoryMap[addr];

	if ((realAddress >= 0x2000 && realAddress <= 0x2007) || (realAddress >= 0x4000 && realAddress <= 0x401F))
	{
		// IO register
		SetIORegister(addr, val);
	}
	else if (realAddress >= 0x8000)
	{
		// Write to PRG RAM
		mapper->WritePRGByte(realAddress, val);
	}
	else
	{
		memory[realAddress] = val;
	}
}

void Memory::SetIORegister(uint16_t addr, uint8_t val)
{
	switch (addr)
	{
	case 0x2000:
		// PPUCTRL
		ppu->SetPPUCTRL(val);
		break;
	case 0x2001:
		// PPUMASK
		ppu->SetPPUMASK(val);
		break;
	case 0x2003:
		// OAMADDR
		ppu->SetOAMADDR(val);
		break;
	case 0x2004:
		// OAMDATA
		ppu->SetOAMDATA(val);
		break;
	case 0x2005:
		// PPUSCROLL
		ppu->SetPPUSCROLL(val);
		break;
	case 0x2006:
		// PPUADDR
		ppu->SetPPUADDR(val);
		break;
	case 0x2007:
		// PPUDATA
		ppu->SetPPUDATA(val);
		break;
	case 0x4014:
		// OAMDMA
		ppuOAMDMAActive = true;
		curPPUOAMDMACycle = 0;
		ppuOAMDMAAddr = ((uint16_t)val << 8);
		break;
	case 0x4016:
		InputManager::GetInstance()->WriteController(0, val);
		break;
	case 0x4017:
		InputManager::GetInstance()->WriteController(1, val);
		break;
	default:
		apu->WriteRegister(addr, val);
		break;
	}
}

void Memory::SetWord(uint16_t addr, uint16_t val)
{
	SetByte(addr, val & 0xFF);
	SetByte(addr + 1, (val >> 8));
}

void Memory::PPUOAMDMACycle()
{
	if (curPPUOAMDMACycle > 1)
	{
		if ((curPPUOAMDMACycle % 2) == 0)
		{
			ppuOAMDMAByte = ReadByte(ppuOAMDMAAddr);
			ppuOAMDMAAddr++;
		}
		else
		{
			ppu->SetOAMByte(ppuOAMDMAByte);
		}
	}
	curPPUOAMDMACycle++;

	if (curPPUOAMDMACycle == 514)
	{
		ppuOAMDMAActive = false;
	}
}

void Memory::DumpRAM()
{
	std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream filePathSS;
	filePathSS << "ramdump_" << std::ctime(&curTime) << ".hex";
	std::string filePath = filePathSS.str();
	std::replace(filePath.begin(), filePath.end(), ':', '-');
	filePath.erase(std::remove(filePath.begin(), filePath.end(), '\n'), filePath.end());
	filePath.erase(std::remove(filePath.begin(), filePath.end(), '\r'), filePath.end());
	std::ofstream outputFile(filePath, std::ios::binary);

	if (outputFile.is_open())
	{
		for (int i = 0; i < MEMORY_AMOUNT; i++)
		{
			outputFile << ReadByte(i);
		}

		outputFile.flush();
		outputFile.close();
	}
}
