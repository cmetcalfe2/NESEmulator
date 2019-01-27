#include "PPU.h"

uint8_t PPU::ReadPPUSTATUS()
{
	tempVRAMAddr = 0x0000;
	uint8_t status = (leastSigBitsLastWrittenToReg | (spriteOverflowFlag << 5) | (spriteZeroHitFlag << 6) | (vblankStartedFlag << 7));
	vblankStartedFlag = 0;
	return status;
}

uint8_t PPU::ReadOAMDATA()
{
	return primaryOAM[oamAddr];
}

uint8_t PPU::ReadPPUDATA()
{
	uint8_t val = vram[curVRAMAddr];
	if ((curScanline > 239 && curScanline != 261) | !renderingEnabled)
	{
		curVRAMAddr += ppuDataAddressIncrement;
	}
	else
	{
		IncCurVRAMAddrHorizontal();
		IncCurVRAMAddrVertical();
	}
	return val;
}


void PPU::SetPPUCTRL(uint8_t val)
{
	ppuCtrl = val;
	baseNametableAddress = 0x2000 + ((val & 0x03) * 0x0400);
	ppuDataAddressIncrement = ((val & 0x04) == 0x04) ? 32 : 1;
	spritePatternTableAddr = ((val & 0x08) == 0x08) ? 0x1000 : 0x0000;
	bgPatternTableAddr = ((val & 0x10) == 0x10) ? 0x1000 : 0x0000;
	eightBySixteenSpriteMode = (val & 0x20);
	ppuSlave = (val & 0x40);
	nmiEnabled = (val & 0x80);
	tempVRAMAddr &= (0x0C00);
	tempVRAMAddr |= (((uint16_t)val & 0x03) << 10);
}

void PPU::SetPPUMASK(uint8_t val)
{
	greyscaleEnabled = (val & 0x01);
	leftMostBGEnabled = (val & 0x02);
	leftMostSpritesEnabled = (val & 0x04);
	showBG = (val & 0x08);
	showSprites = (val & 0x10);
	emphasizeRed = (val & 0x20);
	emphasizeGreen = (val & 0x40);
	emphasizeBlue = (val & 0x80);

	renderingEnabled = (val & 0x18);
}

void PPU::SetOAMADDR(uint8_t val)
{
	oamAddr = val;
}

void PPU::SetOAMDATA(uint8_t val)
{
	if (curScanline > 239)
	{
		// Ignore writes during rendering
		primaryOAM[oamAddr] = val;
		oamAddr++;
	}
}

void PPU::SetPPUSCROLL(uint8_t val)
{
	if (!ppuRegSecondWrite)
	{
		tempVRAMAddr = (tempVRAMAddr & 0xFFE0) | ((val & 0xF8) >> 3); // First 5 bits of val = coarse X scroll
		fineXScroll = (val & 0x07); // Lst 3 bits of val = fine X scroll
		ppuRegSecondWrite = true;
	}
	else
	{
		tempVRAMAddr = (tempVRAMAddr & 0x0FFF) | (((uint16_t)val & 0x07) << 13); // Last 3 bits of val = fine Y scroll
		tempVRAMAddr = (tempVRAMAddr & 0xFC1F) | (((uint16_t)val & 0xF8) << 2); // First 5 bits of val = coarse Y scroll
		ppuRegSecondWrite = false;
	}
}

void PPU::SetPPUADDR(uint8_t val)
{
	if (!ppuRegSecondWrite)
	{
		tempVRAMAddr = (tempVRAMAddr & 0x00FF) | (((uint16_t)val & 0x3F) << 8); // First write = high byte (only 6 bits for max 16KB addressable VRAM)
		tempVRAMAddr &= 0xBFFF; // Bit 15 = 0
		ppuRegSecondWrite = true;
	}
	else
	{
		tempVRAMAddr = (tempVRAMAddr & 0xFF00) | val; // Second write = low byte
		curVRAMAddr = tempVRAMAddr; // v = t
		ppuRegSecondWrite = false;
	}
}

void PPU::SetPPUDATA(uint8_t val)
{
	//vram[curVRAMAddr] = val;
	SetVRAMByte(curVRAMAddr, val);
	if ((curScanline > 239 && curScanline != 261) | !renderingEnabled)
	{
		curVRAMAddr += ppuDataAddressIncrement;
	}
	else
	{
		IncCurVRAMAddrHorizontal();
		IncCurVRAMAddrVertical();
	}
}
