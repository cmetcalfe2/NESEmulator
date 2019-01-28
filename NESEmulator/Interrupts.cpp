#include "Interrupts.h"

void Interrupts::AssertIntertupt(InterruptType interruptType)
{
	switch (interruptType)
	{
	case INTERRUPT_RST:
		rstPending = true;
		break;
	case INTERRUPT_IRQ:
		irqLinePowered = true;
		break;
	case INTERRUPT_NMI:
		nmiLinePowered = true;
		break;
	}
}

InterruptType Interrupts::PollEdgeDetectors()
{
	if (rstPending)
	{
		Reset();
		return INTERRUPT_RST;
	}
	else if (nmiEdgeDetectorPowered)
	{
		nmiEdgeDetectorPowered = false;
		nmiLinePowered = false;
		return INTERRUPT_NMI;
	}
	else if (irqLinePowered)
	{
		return INTERRUPT_IRQ;
	}
	else
	{
		return INTERRUPT_NONE;
	}
}

void Interrupts::InterruptHijacking(InterruptType& interruptBeingHandled)
{
	if (nmiEdgeDetectorPowered && ((interruptBeingHandled == INTERRUPT_IRQ) || (interruptBeingHandled == INTERRUPT_BRK)))
	{
		nmiEdgeDetectorPowered = false;
		nmiLinePowered = false;
		interruptBeingHandled = INTERRUPT_NMI;
	}
	else if (irqLinePowered && (interruptBeingHandled == INTERRUPT_BRK))
	{
		interruptBeingHandled = INTERRUPT_IRQ;
	}
}

void Interrupts::PollInterruptLines()
{
	irqEdgeDetectorPowered = irqLinePowered;
	
	nmiEdgeDetectorPowered = nmiLinePowered;
}

void Interrupts::Reset()
{
	rstPending = false;

	nmiEdgeDetectorPowered = false;
	irqEdgeDetectorPowered = false;

	nmiLinePowered = false;
	irqLinePowered = false;
}
