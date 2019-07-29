#include "PPU.h"


PPU::PPU(Memory* m, Interrupts* i)
{
	mem = m;
	interrupts = i;
	Init();
}


PPU::~PPU()
{
	
}

void PPU::Init()
{
	InitVRAMMap();
	InitCycleFunctionVectors();
	LoadPaletteFile();
	Reset();
}

void PPU::Reset()
{
	curPixel = 0;
	curScanline = 261;
	oddFrame = false;

	// Registers
	SetPPUCTRL(0x00);
	SetPPUMASK(0x00);
	ppuStatus = 0x00;
	SetOAMADDR(0x00);
	SetPPUSCROLL(0x00);
	SetPPUADDR(0x00);
	//SetPPUDATA(0x00);
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
		else if (i == 0x3F10 || i == 0x3F14 || i == 0x3F18 || i == 0x3F1C)
		{
			vramMap[i] = i - 0x0010;
		}
		else if (i >= 0x3F20 && i <= 0x3FFF)
		{
			vramMap[i] = (i % 0x0020) + 0x3F00;
		}
		else
		{
			vramMap[i] = i;
		}
	}

	// Init nametable maps
	for (uint16_t i = 0; i < 0x0400; i++)
	{
		nametableL1Map[i] = 0x2000 + i;
		nametableL2Map[i] = 0x2400 + i;
		nametableL3Map[i] = 0x2800 + i;
		nametableL4Map[i] = 0x2C00 + i;
	}
}

void PPU::LoadPaletteFile()
{
	std::ifstream input("nespalette.pal", std::ios::binary | std::ios::ate);

	if (!input.fail())
	{
		unsigned int inputSize = input.tellg();

		if (inputSize != 192)
		{
			printf(".pal file is invalid size (should be 192 bytes)\n");
		}

		input.seekg(0, std::ios::beg);

		for (int i = 0; i < 64; i++)
		{
			nesPalette[i].r = input.get();
			nesPalette[i].g = input.get();
			nesPalette[i].b = input.get();
		}
	}
	else
	{
		printf("Failed to open nespalette.pal\n");
	}
}

uint8_t PPU::ReadVRAMByte(uint16_t address)
{
	if (address >= 0x4000)
	{
		return 0xFF;
	}

	uint16_t realAddress = GetRealVRAMAddress(address);
	
	if (realAddress < 0x2000)
	{
		return mapper->ReadCHRByte(realAddress);
	}
	else
	{
		return vram[realAddress];
	}
}

void PPU::SetVRAMByte(uint16_t address, uint8_t val)
{
	if (address >= 0x4000)
	{
		return;
	}

	uint16_t realAddress = GetRealVRAMAddress(address);
	
	if (realAddress < 0x2000)
	{
		mapper->WriteCHRByte(realAddress, val);
	}
	else
	{
		vram[realAddress] = val;
	}
}

uint16_t PPU::GetRealVRAMAddress(uint16_t address)
{
	uint16_t realAddress = address;
	/*if (realAddress >= 0x4000)
	{
		realAddress %= 0x4000;
	}*/
	while (realAddress != vramMap[realAddress])
	{
		realAddress = vramMap[realAddress];
	}
	return realAddress;
}

void PPU::DumpVRAM()
{
	std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::stringstream filePathSS;
	filePathSS << "vramdump_" << std::ctime(&curTime) << ".hex";
	std::string filePath = filePathSS.str();
	std::replace(filePath.begin(), filePath.end(), ':', '-');
	filePath.erase(std::remove(filePath.begin(), filePath.end(), '\n'), filePath.end());
	filePath.erase(std::remove(filePath.begin(), filePath.end(), '\r'), filePath.end());
	std::ofstream outputFile(filePath, std::ios::binary);

	if (outputFile.is_open())
	{
		for (int i = 0; i < 0x4000; i++)
		{
			outputFile << ReadVRAMByte(i);
		}

		outputFile.flush();
		outputFile.close();
	}
}

void PPU::SetOAMByte(uint8_t val)
{
	primaryOAM[oamAddr] = val;
	oamAddr++;
}

void PPU::SetMirrorMode(NametableMirroringMode mode)
{
	unsigned int nametableSize = 0x0400 * sizeof uint16_t;
	switch (mode)
	{
	case MIRRORING_HORIZONTAL:
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL3Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL3Map[0], nametableSize);
		break;
	case MIRRORING_VERTICAL:
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL2Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL2Map[0], nametableSize);
		break;
	case MIRRORING_SINGLESCREEN:
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL1Map[0], nametableSize);
		break;
	case MIRRORING_4SCREEN:
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL2Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL3Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL4Map[0], nametableSize);
		break;
	}
}

void PPU::InitCycleFunctionVectors()
{
	for (int x = 0; x < 341; x++)
	{
		for (int y = 0; y < 262; y++)
		{
			uint32_t cycleFunctionMask = 0;

			if (y < 240 || y == 261)
			{

				bool isPreRenderLine = (y == 261);

				if (x > 0 && x <= 256)
				{
					if (!isPreRenderLine)
					{
						// Render pixel
						cycleFunctionMask |= BCIM_RENDERPIXEL;
					}
				}

				// Visible lines / Prerender line
				if ((x > 0 && x <= 256) || (x >= 321 && x <= 336))
				{
					cycleFunctionMask |= BCIM_SHIFTBGREGISTERS;

					int subCycle = x % 8;
					switch (subCycle)
					{
					case 0:
						cycleFunctionMask |= BCIM_INCRVRAMHORIZONTAL;
						if (x == 256)
						{
							cycleFunctionMask |= BCIM_INCVRAMVERTICAL;
						}
						cycleFunctionMask |= BCIM_FEEDBGREGISTERS;
						break;
					case 1:
						cycleFunctionMask |= BCIM_FETCHNTBYTE;
						if (x == 1 && isPreRenderLine)
						{
							cycleFunctionMask |= BCIM_CLEARFLAGS;
						}
						break;
					case 3:
						cycleFunctionMask |= BCIM_FETCHATBYTE;
						break;
					case 5:
						cycleFunctionMask |= BCIM_FETCHLOWBGBYTE;
						break;
					case 7:
						cycleFunctionMask |= BCIM_FETCHHIGHBGBYTE;
						break;
					}
				}

				if (x == 257)
				{
					// hori v = hori t
					cycleFunctionMask |= BCIM_COPYVRAMHORIZONTAL;
				}

				if (isPreRenderLine && x >= 280 && x <= 304)
				{
					// vert v = vert t
					cycleFunctionMask |= BCIM_COPYVRAMVERTICAL;
				}

				if (x == 337 || x == 339)
				{
					cycleFunctionMask |= BCIM_FETCHNTBYTE;
				}
			}
			else if (y == 241 && x == 1)
			{
				cycleFunctionMask |= BCIM_SETVBLANKFLAG;
			}

			cycleFunctionMasks[x][y] = cycleFunctionMask;

		}
	}
}

void PPU::Cycle()
{
	/*int numCycleFunctions = numFunctionsInCycle[curPixel][curScanline];

	if (numCycleFunctions > 0)
	{
		CycleFunction* firstFunctionPtr = &cycleFunctions[curPixel][curScanline][0];
		for (int i = 0; i < numCycleFunctions; i++)
		{
			//(this->*cycleFunctions[curPixel][curScanline][i])();
			(this->*firstFunctionPtr[i])();
		}
	}*/

	/* TEST */
	int x = curPixel;
	int y = curScanline;

	if (y < 240 || y == 261)
	{

		bool isPreRenderLine = (y == 261);

		if (x > 0 && x <= 256)
		{
			if (!isPreRenderLine)
			{
				// Render pixel
				RenderPixel();
			}
		}

		// Visible lines / Prerender line
		if ((x > 0 && x <= 256) || (x >= 321 && x <= 336))
		{
			ShiftBGRegisters();

			int subCycle = x % 8;
			switch (subCycle)
			{
			case 0:
				IncCurVRAMAddrHorizontal();
				if (x == 256)
				{
					IncCurVRAMAddrVertical();
				}
				FeedBGRegisters();
				break;
			case 1:
				FetchNTByte();
				if (x == 1 && isPreRenderLine)
				{
					ClearFlags();
				}
				break;
			case 3:
				FetchATByte();
				break;
			case 5:
				FetchLowBGByte();
				break;
			case 7:
				FetchHighBGByte();
				break;
			}
		}

		if (x == 257)
		{
			// hori v = hori t
			CopyVRAMAddrHorizontal();
		}

		if (isPreRenderLine && x >= 280 && x <= 304)
		{
			// vert v = vert t
			CopyVRAMAddrVertical();
		}

		if (x == 337 || x == 339)
		{
			FetchNTByte();
		}
	}
	else if (y == 241 && x == 1)
	{
		SetVBlankStartedFlag();
	}

	/* END TEST */

	//BGCycle();

	if (renderingEnabled && (curScanline < 240 || curScanline == 261))
	{
		SpriteEvaluationCycle();
	}

	curPixel++;
	if (curPixel == 341)
	{	
		curPixel = 0;
		curScanline++;

		if (curScanline == 262)
		{
			curScanline = 0;

			if (!oddFrame || !showBG)
			{
				curPixel = 0;
			}
			else
			{
				curPixel = 1; // Skip first cycle (idle) on odd frames when BG rendering is enabled
			}

			oddFrame = !oddFrame;
		}
	}
}

void PPU::SpriteEvaluationCycle()
{
	bool oddCycle = (curPixel % 2);
	if (curPixel > 0 && curPixel < 65)
	{
		// Initialise secondary OAM to 0xFF
		if (oddCycle)
		{
			oamData = 0xFF;
		}
		if (!oddCycle)
		{
			secondaryOAM[curPixel / 2] = oamData;
		}
	}
	else if (curPixel < 257)
	{
		// Sprite evaluation
		if (curPixel == 65)
		{
			// Reset vars
			baseOAMAddr = oamAddr;
			spriteEvalN = 0;
			spriteEvalM = 0;
			spriteEvalO = 0;

			copyingSprite = false;
			secondaryOAMIndex = 0;
			numSpritesFound = 0;
			allSpritesEvaluated = false;
		}

		if (oddCycle)
		{
			if (spriteEvalN > 63)
			{
				allSpritesEvaluated = true;
				spriteEvalN = 0;
			}

			oamData = primaryOAM[baseOAMAddr + (spriteEvalN * 4) + spriteEvalM];

		}
		else
		{
			if (numSpritesFound < 8)
			{
				if (IsSpriteYInRange())
				{
					copyingSprite = true;
				}
				if (copyingSprite)
				{
					secondaryOAM[secondaryOAMIndex] = oamData;
					secondaryOAMIndex++;
					spriteEvalM++;
					if (spriteEvalM > 3)
					{
						spriteEvalM = 0;
						copyingSprite = false;
						spriteIndices[numSpritesFound] = spriteEvalN;
						numSpritesFound++;
						spriteEvalN++;
					}
				}
				else
				{
					spriteEvalN++;
				}
			}
			else if (!allSpritesEvaluated)
			{
				if (IsSpriteYInRange() && !copyingSprite)
				{
					copyingSprite = true;
					spriteOverflowFlag = 1;
					spriteEvalO = 0;
				}
				else if (copyingSprite)
				{
					spriteEvalM++;
					spriteEvalO++;
					if (spriteEvalM > 3)
					{
						spriteEvalM = 0;
						spriteEvalN++;
					}
					if (spriteEvalO > 3)
					{
						spriteEvalO = 0;
						copyingSprite = false;
					}
				}
				else
				{
					spriteEvalN++;
					spriteEvalM++;
					
					if (spriteEvalM > 3)
					{
						spriteEvalM = 0;
					}
				}
			}
			else
			{
				spriteEvalM = 0;
				spriteEvalN++;
			}
		}
	}
	else if (curPixel == 257)
	{
		secondaryOAMIndex = 0;
		LoadSpritesIntoRegisters();
	}
}

void* PPU::GetPixelBuffer()
{
	switchPixelBuffers = false;
	return &renderedPixelBuffers[renderedPixelBuffer];
}
