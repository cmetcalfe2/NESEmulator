#include "NES.h"



NES::NES()
{
	memory = new Memory();
	processor = new CPU(memory);
}


NES::~NES()
{
	delete processor;
	processor = NULL;

	delete memory;
	memory = NULL;
}


void NES::LoadROM(std::string path)
{
	std::ifstream input(path, std::ios::binary | std::ios::ate);

	if (!input.fail())
	{
		unsigned int inputSize = input.tellg();
		std::cout << inputSize << std::endl;
		input.seekg(0, std::ios::beg);
		char test[4];
		test[3] = '\0';

		for (int i = 0; i < 3; i++)
		{
			test[i] = input.get();
			// Start loading program to memory location 0x200
			//memory[512 + i] = input.get();
		}
		std::cout << test << std::endl;

		processor->ps.UnSetI();

		std::cout << (int)processor->ps.N() << std::endl;
		std::cout << (int)processor->ps.V() << std::endl;
		std::cout << (int)processor->ps.B() << std::endl;
		std::cout << (int)processor->ps.D() << std::endl;
		std::cout << (int)processor->ps.I() << std::endl;
		std::cout << (int)processor->ps.Z() << std::endl;
		std::cout << (int)processor->ps.C() << std::endl;

		if (processor->ps.N() == 1)
		{
			std::cout << "N flag is 1" << std::endl;
		}

		//return true;
	}
	else
	{
		printf("Failed to load program - %s\n", path.c_str());
		//return false;
	}
}
