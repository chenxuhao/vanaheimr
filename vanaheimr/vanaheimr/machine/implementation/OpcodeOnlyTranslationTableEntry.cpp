/*! \file   OpcodeOnlyTranslationTableEntry.cpp
	\date   Wednesday February 1, 2012
	\author Gregory Diamos <gdiamos@nvidia.com>
	\brief  The source file for the OpcodeOnlyTranslationTableEntry class.
*/

// Vanaheimr Includes
#include <vanaheimr/machine/interface/OpcodeOnlyTranslationTableEntry.h>
#include <vanaheimr/machine/interface/MachineModel.h>
#include <vanaheimr/machine/interface/Instruction.h>

#include <vanaheimr/compiler/interface/Compiler.h>

namespace vanaheimr
{

namespace machine
{

OpcodeOnlyTranslationTableEntry::OpcodeOnlyTranslationTableEntry(
	const std::string& s, const std::string& d)
: TranslationTableEntry(s), machineInstructionOpcode(d)
{

}

static const Operation* getOrAddOperation(const std::string& opcode)
{
	auto compiler = vanaheimr::compiler::Compiler::getSingleton();

	auto operation = compiler->getMachineModel()->getOperation(opcode);

	if(operation == nullptr)
	{
		compiler->getMachineModel()->addOperation(Operation(opcode));
		
		operation = compiler->getMachineModel()->getOperation(opcode);
	}

	return operation;
}

OpcodeOnlyTranslationTableEntry::MachineInstructionVector
	OpcodeOnlyTranslationTableEntry::translateInstruction(
	const ir::Instruction* instruction) const
{
	auto machineInstruction = new Instruction(
		getOrAddOperation(machineInstructionOpcode), instruction->block);
	
	for(auto read : instruction->reads)
	{
		machineInstruction->appendRead(read->clone());
	}
	
	for(auto write : instruction->writes)
	{
		machineInstruction->appendWrite(write->clone());
	}
	
	MachineInstructionVector instructions;
	
	instructions.push_back(machineInstruction);
	
	return instructions;
}

TranslationTableEntry* OpcodeOnlyTranslationTableEntry::clone() const
{
	return new OpcodeOnlyTranslationTableEntry(*this);
}

}

}



