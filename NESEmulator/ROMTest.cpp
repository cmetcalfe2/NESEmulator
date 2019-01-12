#include "ROMTest.h"



ROMTest::ROMTest()
{
}


ROMTest::~ROMTest()
{
}

void ROMTest::Start()
{
	bool romLoaded = nes.LoadROM(romPath);

	if (romLoaded)
	{
		while (!finishExecuting)
		{
			nes.Cycle();

			if (nes.processor->GetCyclesElapsed() > 0)
			{
				PostCycle();
			}
		}
	}

	std::string test;
	std::cin >> test;
}
