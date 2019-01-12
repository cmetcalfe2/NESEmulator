#include "CPUTestBlargg.h"

int main()
{
	/*NES nes;
	bool romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/instr_test-v5/official_only.nes");

	if (romLoaded)
	{
		while (true)
		{
			nes.Cycle();
		}
	}

	std::string test;
	std::cin >> test;*/

	CPUTestBlargg test;
	test.Start();

	return 0;
}