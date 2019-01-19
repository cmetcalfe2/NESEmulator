#include "MapperMMC1.h"



MapperMMC1::MapperMMC1(PPU* p) : Mapper(p)
{
	prgBankOffset1 = 0x0000;
	prgBankOffset2 = (0x4000 * 15);

	regShift = 0b00010000;
	regControl = 0b00000110;
}

MapperMMC1::~MapperMMC1()
{
}

void MapperMMC1::WritePRGByte(uint32_t address, uint8_t val)
{
	SetPRGByte(address, val);

	if (writesIgnored)
	{
		return;
	}
	else
	{
		writesIgnored = true; // Ignore subsequent writes within same instruction
	}

	if (val & 0x80)
	{
		// Bit 7 set, reset shift reg
		regShift = 0b00010000;
		writeCounter = 0;
	}
	else
	{
		writeCounter++;
		regShift >>= 1;
		regShift |= ((val & 1) << 4);

		if (writeCounter == 5)
		{
			if (address < 0xA000)
			{
				SetControlReg();
			}
			else if (address < 0xC000)
			{
				SetCHR1Reg();
			}
			else if (address < 0xE000)
			{
				SetCHR2Reg();
			}
			else
			{
				SetPRGReg();
			}

			regShift = 0b00010000;
			writeCounter = 0;
		}
	}
}

void MapperMMC1::SetControlReg()
{
	regControl = regShift;

	prgMode = static_cast<PRGBankMode>((regControl >> 2) & 3);

	SwitchPRGBank();
}

void MapperMMC1::SetCHR1Reg()
{
	regCHR1 = regShift;
}

void MapperMMC1::SetCHR2Reg()
{
	regCHR2 = regShift;
}

void MapperMMC1::SetPRGReg()
{
	regPRG = regShift;

	SwitchPRGBank();
}

void MapperMMC1::SwitchPRGBank()
{
	if (prgMode == PRGMODE_BOTH)
	{
		prgBankOffset1 = (regPRG & 0xE) * 0x4000;
		prgBankOffset2 = prgBankOffset1 + 0x4000;
	}
	else if (prgMode == PRGMODE_SWITCHLAST)
	{
		prgBankOffset1 = 0;
		prgBankOffset2 = (regPRG & 0xF) * 0x4000;
	}
	else
	{
		prgBankOffset1 = (regPRG & 0xF) * 0x4000;
		prgBankOffset2 = 0;
	}
}

void MapperMMC1::OnInstructionFinished()
{
	writesIgnored = false;
}
