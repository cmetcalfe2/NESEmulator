#include "PPU.h"


bool PPU::IsSpriteYInRange()
{
	if (eightBySixteenSpriteMode)
	{
		if (curScanline >= oamData && curScanline < (oamData + 16) && oamData < 0xF0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (curScanline >= oamData && curScanline < (oamData + 8) && oamData < 0xF0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

void PPU::LoadSpritesIntoRegisters()
{
	// Load 8 sprites from secondary OAM
	uint8_t yPos;
	uint8_t tileNumber;
	for (int i = 0; i < 8; i++)
	{
		yPos = secondaryOAM[(i * 4)];
		tileNumber = secondaryOAM[(i * 4) + 1];
		spriteAttributes[i] = secondaryOAM[(i * 4) + 2];
		spriteXPositionCounters[i] = secondaryOAM[(i * 4) + 3] + 1;

		// Get bitmap value
		uint8_t row = curScanline - yPos;
		if (spriteAttributes[i] & 0x80)
		{
			// vertically flipped
			row = spriteHeight - (row + 1);
		}

		if (!eightBySixteenSpriteMode)
		{
			uint16_t rowLowAddress = spritePatternTableAddr + ((uint16_t)tileNumber * 16) + row;
			spriteBitmaps[i][1] = ReadVRAMByte(rowLowAddress);
			spriteBitmaps[i][0] = ReadVRAMByte(rowLowAddress + 8);
		}
		else
		{
			uint16_t rowLowAddress = ((tileNumber & 1) * 0x1000) + (((uint16_t)tileNumber >> 1) * 32) + row;
			spriteBitmaps[i][1] = ReadVRAMByte(rowLowAddress);
			spriteBitmaps[i][0] = ReadVRAMByte(rowLowAddress + 16);
		}
	}
}
