#include "CPUTestBlargg.h"



CPUTestBlargg::CPUTestBlargg()
{
	romPath = "C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/instr_test-v5/official_only.nes";
}


CPUTestBlargg::~CPUTestBlargg()
{
}

void CPUTestBlargg::PostCycle()
{
	if (nes.processor->IsInstructionFinished())
	{
		uint8_t status = nes.memory->ReadByte(0x6000);

		if (status != 0x80)
		{
			//finishExecuting = true;
			// Test finished
			char textBuffer[2048];
			char lastChar = 'a';
			uint16_t memPointer = 0x6004;
			int textBufferIndex = 0;

			while (lastChar != 0x00)
			{
				lastChar = nes.memory->ReadByte(memPointer);
				memPointer++;

				textBuffer[textBufferIndex] = lastChar;
				textBufferIndex++;
			}

			//printf("%02X %s\n", status, textBuffer);
		}
	}
}
