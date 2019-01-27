#include "Logger.h"


void Logger::Init()
{

}

void Logger::LogCPUInstruction(uint16_t pc, uint8_t opcode, uint8_t operand1, uint8_t operand2, uint16_t address, uint8_t a, uint8_t x, uint8_t y, uint8_t sp, uint8_t ps, unsigned long cycles, std::string instructionName, bool hasTwoOperands)
{
	if (hasTwoOperands)
		printf("%04X  %02X %02X %02X   %s     A:%02X X:%02X Y:%02X SP:%02X  PS:%02X  CYC:%i\n", pc, opcode, operand1, operand2, instructionName.c_str(), a, x, y, sp, ps, cycles);
	else
		printf("%04X  %02X %02X      %s     A:%02X X:%02X Y:%02X SP:%02X  PS:%02X  CYC:%i\n", pc, opcode, operand1, instructionName.c_str(), a, x, y, sp, ps, cycles);
}