#include "NES.h"

int main()
{
	NES nes;
	bool romLoaded = nes.LoadROM("C:/Users/Craig/Documents/Projects/NESEmulator/ROMs/nestest.nes");

	if (romLoaded)
	{
		while (true)
		{
			nes.Cycle();
		}
	}

	std::string test;
	std::cin >> test;

	return 0;
}