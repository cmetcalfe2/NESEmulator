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
		/*std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2000]);
		std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2400]);
		std::copy(nametableL3Map, nametableL3Map + nametableSize, vramMap[0x2800]);
		std::copy(nametableL3Map, nametableL3Map + nametableSize, vramMap[0x2C00]);*/
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL3Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL3Map[0], nametableSize);
		break;
	case MIRRORING_VERTICAL:
		/*std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2000]);
		std::copy(nametableL2Map, nametableL2Map + nametableSize, vramMap[0x2400]);
		std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2800]);
		std::copy(nametableL2Map, nametableL2Map + nametableSize, vramMap[0x2C00]);*/
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL2Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL2Map[0], nametableSize);
		break;
	case MIRRORING_SINGLESCREEN:
		/*std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2000]);
		std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2400]);
		std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2800]);
		std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2C00]);*/
		memcpy(&vramMap[0x2000], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2400], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2800], &nametableL1Map[0], nametableSize);
		memcpy(&vramMap[0x2C00], &nametableL1Map[0], nametableSize);
		break;
	case MIRRORING_4SCREEN:
		/*std::copy(nametableL1Map, nametableL1Map + nametableSize, vramMap[0x2000]);
		std::copy(nametableL2Map, nametableL2Map + nametableSize, vramMap[0x2400]);
		std::copy(nametableL3Map, nametableL3Map + nametableSize, vramMap[0x2800]);
		std::copy(nametableL4Map, nametableL4Map + nametableSize, vramMap[0x2C00]);*/
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
			std::vector<CycleFunction>* functionVector = &cycleFunctions[x][y];

			if (y < 240 || y == 261)
			{

				bool isPreRenderLine = (y == 261);

				if (x > 0 && x <= 256)
				{
					if (!isPreRenderLine)
					{
						// Render pixel
						functionVector->push_back(&PPU::RenderPixel);
					}
				}

				// Visible lines / Prerender line
				if ((x > 0 && x <= 256) || (x >= 321 && x <= 336))
				{
					functionVector->push_back(&PPU::ShiftBGRegisters);

					int subCycle = x % 8;
					switch (subCycle)
					{
					case 0:
						functionVector->push_back(&PPU::IncCurVRAMAddrHorizontal);
						if (x == 256)
						{
							functionVector->push_back(&PPU::IncCurVRAMAddrVertical);
						}
						functionVector->push_back(&PPU::FeedBGRegisters);
						break;
					case 1:
						functionVector->push_back(&PPU::FetchNTByte);
						if (x == 1 && isPreRenderLine)
						{
							functionVector->push_back(&PPU::ClearFlags);
						}
						break;
					case 3:
						functionVector->push_back(&PPU::FetchATByte);
						break;
					case 5:
						functionVector->push_back(&PPU::FetchLowBGByte);
						break;
					case 7:
						functionVector->push_back(&PPU::FetchHighBGByte);
						break;
					}
				}

				if (x == 257)
				{
					// hori v = hori t
					functionVector->push_back(&PPU::CopyVRAMAddrHorizontal);
				}

				if (isPreRenderLine && x >= 280 && x <= 304)
				{
					// vert v = vert t
					functionVector->push_back(&PPU::CopyVRAMAddrVertical);
				}

				if (x == 337 || x == 339)
				{
					functionVector->push_back(&PPU::FetchNTByte);
				}
			}
			else if (y == 241 && x == 1)
			{
				functionVector->push_back(&PPU::SetVBlankStartedFlag);
			}

			numFunctionsInCycle[x][y] = functionVector->size();

		}
	}
}

void PPU::Cycle()
{
	int numCycleFunctions = numFunctionsInCycle[curPixel][curScanline];

	if (numCycleFunctions > 0)
	{
		CycleFunction* firstFunctionPtr = &cycleFunctions[curPixel][curScanline][0];
		for (int i = 0; i < numCycleFunctions; i++)
		{
			//(this->*cycleFunctions[curPixel][curScanline][i])();
			(this->*firstFunctionPtr[i])();
		}
	}

	SpriteEvaluationCycle();

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

}

void* PPU::GetPixelBuffer()
{
	switchPixelBuffers = false;
	return &renderedPixelBuffers[renderedPixelBuffer];
}
