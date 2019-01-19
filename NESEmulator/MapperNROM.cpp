#include "MapperNROM.h"



MapperNROM::MapperNROM(PPU* p) : Mapper(p)
{
}


MapperNROM::~MapperNROM()
{
}


void MapperNROM::OnRPRGROMLoaded()
{
	prgBankOffset1 = 0x0000;
	if (numPRGBanks == 1)
	{
		prgBankOffset2 = 0x0000;
	}
	else
	{
		prgBankOffset2 = 0x4000;
	}
}
