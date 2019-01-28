#include "PPU.h"

void PPU::FetchNTByte()
{
	if (renderingEnabled)
	{
		uint16_t ntAddress = 0x2000 | (curVRAMAddr & 0x0FFF);
		ntByte = ReadVRAMByte(ntAddress);
	}
}

void PPU::FetchATByte()
{
	if (renderingEnabled)
	{
		atByte = ReadVRAMByte(0x23C0 | (curVRAMAddr & 0x0C00) | ((curVRAMAddr >> 4) & 0x38) | ((curVRAMAddr >> 2) & 0x07));

		uint8_t tileColumn = curVRAMAddr & bitMaskCoarseX;
		uint8_t tileRow = (curVRAMAddr & bitMaskCoarseY) >> 5;
		uint8_t quadrantID = (tileRow & 0x2) + ((tileColumn & 0x2) >> 1);
		uint8_t atBits = (atByte & (0x3 << (quadrantID * 2))) >> (quadrantID * 2);
		atLowLatch = (atBits & 1) ? 0xFF : 0;
		atHighLatch = (atBits & 2) ? 0xFF : 0;
	}
}

void PPU::FetchLowBGByte()
{
	if (renderingEnabled)
	{
		uint16_t patternAddress = bgPatternTableAddr + ((uint16_t)ntByte * 16) + ((curVRAMAddr & bitMaskFineY) >> 12);
		bgLowByte = ReadVRAMByte(patternAddress);
	}
}

void PPU::FetchHighBGByte()
{
	if (renderingEnabled)
	{
		uint16_t yScroll = ((curVRAMAddr & bitMaskFineY) >> 12);
		uint16_t patternAddress = bgPatternTableAddr + ((uint16_t)ntByte * 16) + yScroll + 8;
		bgHighByte = ReadVRAMByte(patternAddress);
	}
}

void PPU::FeedBGRegisters()
{
	bgPatternRegisters[0] = bgPatternRegisters[0] | (uint16_t)bgLowByte;
	bgPatternRegisters[1] = bgPatternRegisters[1]| (uint16_t)bgHighByte;

	bgAttributeRegisters[0] = (bgAttributeRegisters[0] & 0x00FF) | ((uint16_t)atLowLatch << 8);
	bgAttributeRegisters[1] = (bgAttributeRegisters[1] & 0x00FF) | ((uint16_t)atHighLatch << 8);
}

void PPU::ClearFlags()
{
	vblankStartedFlag = 0;
	spriteOverflowFlag = 0;
	spriteOverflowFlag = 0;
}

void PPU::SetVBlankStartedFlag()
{
	vblankStartedFlag = 1;
	if (nmiEnabled)
	{
		interrupts->AssertIntertupt(INTERRUPT_NMI);
	}
	
	// Switch pixel buffers
	if (activePixelBuffer == 0)
	{
		activePixelBuffer = 1;
		renderedPixelBuffer = 0;
	}
	else
	{
		activePixelBuffer = 0;
		renderedPixelBuffer = 1;
	}
	switchPixelBuffers = true;
}

void  PPU::IncCurVRAMAddrHorizontal()
{
	if (renderingEnabled)
	{
		/*if ((v & 0x001F) == 31) // if coarse X == 31
		  v &= ~0x001F          // coarse X = 0
		  v ^= 0x0400           // switch horizontal nametable
		else
		  v += 1                // increment coarse X*/
		if ((curVRAMAddr & 0x001F) == 31)
		{
			curVRAMAddr &= ~0x001F;
			curVRAMAddr ^= 0x0400;
		}
		else
		{
			curVRAMAddr++;
		}
	}
}

void  PPU::IncCurVRAMAddrVertical()
{
	if (renderingEnabled)
	{
		/*
		if ((v & 0x7000) != 0x7000)        // if fine Y < 7
		  v += 0x1000                      // increment fine Y
		else
		  v &= ~0x7000                     // fine Y = 0
		  int y = (v & 0x03E0) >> 5        // let y = coarse Y
		  if (y == 29)
			y = 0                          // coarse Y = 0
			v ^= 0x0800                    // switch vertical nametable
		  else if (y == 31)
			y = 0                          // coarse Y = 0, nametable not switched
		  else
			y += 1                         // increment coarse Y
		  v = (v & ~0x03E0) | (y << 5)     // put coarse Y back into v
		*/
		if ((curVRAMAddr & 0x7000) != 0x7000)
		{
			curVRAMAddr += 0x1000;
		}
		else
		{
			curVRAMAddr &= ~0x7000;
			uint16_t y = (curVRAMAddr & 0x03E0) >> 5;
			if (y == 29)
			{
				y = 0;
				curVRAMAddr ^= 0x0800;
			}
			else if (y == 31)
			{
				y = 0;
			}
			else
			{
				y += 1;
			}

			curVRAMAddr = (curVRAMAddr & ~0x03E0) | (y << 5);
		}
	}
}

void PPU::CopyVRAMAddrHorizontal()
{
	if (renderingEnabled)
	{
		curVRAMAddr &= ~0x041F;
		curVRAMAddr |= (tempVRAMAddr & 0x041F);
	}
}

void PPU::CopyVRAMAddrVertical()
{
	if (renderingEnabled)
	{
		curVRAMAddr &= ~0x7BE0;
		curVRAMAddr |= (tempVRAMAddr & 0x7BE0);
	}
}

void PPU::RenderPixel()
{
	if (showBG)
	{
		// BG pixel
		uint16_t palletteAddress = 0x3F00;

		uint16_t pattern = ((bgPatternRegisters[0] & (0x8000 >> fineXScroll)) >> (15 - fineXScroll)) | ((bgPatternRegisters[1] & (0x8000 >> fineXScroll)) >> (14 - fineXScroll));
		//paletteIndex |= bgPatternRegisters[0] & 1;
		//paletteIndex |= (bgPatternRegisters[1] & 1) << 1;

		if (pattern != 0)
		{
			uint16_t paletteNum = (bgAttributeRegisters[0] & 1) | ((bgAttributeRegisters[1] & 1) << 1);
			palletteAddress = (0x3F00 + (0x4 * paletteNum));
			palletteAddress += (pattern);
		}

		//paletteIndex |= (bgAttributeRegisters[0] & 1) << 2;
		//paletteIndex |= (bgAttributeRegisters[1] & 1) << 3;

		uint8_t paletteVal = ReadVRAMByte(palletteAddress);

		renderedPixelBuffers[activePixelBuffer][(curScanline * SCREEN_WIDTH) + (curPixel - 1)][3] = nesPalette[paletteVal].r;
		renderedPixelBuffers[activePixelBuffer][(curScanline * SCREEN_WIDTH) + (curPixel - 1)][2] = nesPalette[paletteVal].g;
		renderedPixelBuffers[activePixelBuffer][(curScanline * SCREEN_WIDTH) + (curPixel - 1)][1] = nesPalette[paletteVal].b;
		renderedPixelBuffers[activePixelBuffer][(curScanline * SCREEN_WIDTH) + (curPixel - 1)][0] = 0xFF;
	}
}

void PPU::ShiftBGRegisters()
{
	bgPatternRegisters[1] <<= 1;
	bgPatternRegisters[0] <<= 1;

	bgAttributeRegisters[0] >>= 1;
	bgAttributeRegisters[1] >>= 1;
}
