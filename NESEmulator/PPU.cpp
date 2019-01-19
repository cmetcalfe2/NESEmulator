#include "PPU.h"


PPU::PPU(Memory* m)
{
	mem = m;
	InitVRAMMap();
	curPixel = 0;
	curScanline = 261;
	oddFrame = false;
	renderingEnabled = true;
}


PPU::~PPU()
{
	
}

void PPU::SetMapper(Mapper* m)
{
	mapper = m;
}

void PPU::InitVRAMMap()
{
	for (uint16_t i = 0; i < 0x4000; i++)
	{
		if (i >= 0x3000 && i <= 0x3EFF)
		{
			vramMap[i] = i - 0x1000;
		}
		else if (i >= 0x3F20 && i <= 0x3FFF)
		{
			vramMap[i] = (i % 0x001F) + 0x3F00;
		}
		else
		{
			vramMap[i] = i;
		}
	}
}

uint8_t PPU::ReadVRAMByte(uint16_t address)
{
	uint16_t realAddress = vramMap[address];
	return vram[realAddress];
}

void PPU::SetVRAMByte(uint16_t address, uint8_t val)
{
	uint16_t realAddress = vramMap[address];
	vram[realAddress] = val;
}

void PPU::Cycle()
{
	if (curScanline < 240)
	{
		// Visible lines


		if (renderingEnabled)
		{
			if ((curPixel < 256 || curPixel > 321) && ((curPixel % 8) == 0))
			{
				// Increment horizontal in V
			}
			else if (curPixel == 256)
			{
				// Increment vertical in V
			}
			else if (curPixel == 257)
			{
				// Horizontal V = Horizontal T
			}
		}
	}
	else if (curScanline < 261)
	{
		// Post render/VBlank lines

	}
	else
	{
		// Pre render line

	}

	SpriteEvaluationCycle();

	curPixel++;
	if (curPixel == 341)
	{
		if (!oddFrame)
		{
			curPixel = 0;
		}
		else
		{
			curPixel = 1; // Skip first cycle (idle) on odd frames
		}
		
		curScanline++;
		oddFrame = !oddFrame;

		if (curScanline == 262)
		{
			curScanline = 0;
		}
	}
}

void PPU::SpriteEvaluationCycle()
{

}

