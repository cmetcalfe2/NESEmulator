#include "Mapper.h"
#include "PPU.h"

Mapper::Mapper(PPU* p)
{
	ppu = p;
}


Mapper::~Mapper()
{
}

void Mapper::LoadPRGROM(std::ifstream& inputStream, uint8_t numBanks)
{
	numPRGBanks = numBanks;
	for (uint32_t i = 0; i < (numBanks * 0x4000); i++)
	{
		prgRom[i] = inputStream.get();
	}
}

void Mapper::LoadCHRROM(std::ifstream& inputStream, uint8_t numBanks)
{
	uint8_t realNumBanks = (numBanks == 0) ? 1 : numBanks;
	numCHRBanks = realNumBanks;

	for (uint32_t i = 0; i < (realNumBanks * 0x2000); i++)
	{
		chrRom[i] = (numBanks == 0) ? 0 : inputStream.get();
	}
}

void Mapper::WritePRGByte(uint32_t address, uint8_t val)
{
	SetPRGByte(address, val);
}

uint8_t Mapper::ReadPRGByte(uint32_t address)
{
	if (address < 0xC000)
	{
		// Bank 1
		return prgRom[prgBankOffset1 + (address - 0x8000)];
	}
	else
	{
		// Bank 2
		return prgRom[prgBankOffset2 + (address - 0xC000)];
	}
	//uint32_t bankOffset = (address < 0xC000) ? prgBankOffset1 - 0x8000 : prgBankOffset2 - 0xC000;
	//return prgRom[address + bankOffset];
}

void Mapper::SetPRGByte(uint32_t address, uint8_t val)
{
	if (address == 0xFFFA || address == 0xFFFB)
	{
		int test = 0;
	}
	if (address < 0xC000)
	{
		// Bank 1
		prgRom[prgBankOffset1 + (address - 0x8000)] = val;
	}
	else
	{
		// Bank 2
		prgRom[prgBankOffset2 + (address - 0xC000)] = val;
	}
	//uint32_t bankOffset = (address < 0xC000) ? prgBankOffset1 = 0x8000 : prgBankOffset2 - 0xC000;
	//prgRom[address + bankOffset] = val;
}

void Mapper::WriteCHRByte(uint32_t address, uint8_t val)
{
	SetCHRByte(address, val);
}

uint8_t Mapper::ReadCHRByte(uint32_t address)
{
	//uint32_t bankOffset = (address < 0x1000) ? chrBankOffset1 : chrBankOffset2 - 0x1000;
	return chrRom[address + chrBankOffset1];
}

void Mapper::SetCHRByte(uint32_t address, uint8_t val)
{
	if (chrRAMEnabled)
	{
		//uint32_t bankOffset = (address < 0x1000) ? chrBankOffset1 : chrBankOffset2 - 0x1000;
		chrRom[address + chrBankOffset1] = val;
	}
}
