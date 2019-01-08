#include "NES.h"

int main()
{
	NES nes;
	nes.LoadROM("C:/Users/Craig/Documents/Projects/NES Roms/nestest.nes");

	std::string test;
	std::cin >> test;

	return 0;
}