/*! \file   ArchaeopteryxABI.cpp
	\author Gregory Diamos <gdiamos@nvidia.com>
	\date   Friday December 21, 2021
	\brief  The source file for the ArchaeopteryxABI class.
*/

// Vanaheimr Includes
#include <vanaheimr/abi/interface/ArchaeopteryxABI.h>
#include <vanaheimr/abi/interface/ApplicationBinaryInterface.h>

#include <vanaheimr/ir/interface/Global.h>

#include <vanaheimr/compiler/interface/Compiler.h>

namespace vanaheimr
{

namespace abi
{

ApplicationBinaryInterface* getArchaeopteryxABI()
{
	auto archaeopteryxABI = new ApplicationBinaryInterface;

	// Memory Regions
	archaeopteryxABI->insert(new FixedAddressRegion(
		"parameter", 1024, 8, ir::Global::Shared, 4096));

	// Bound Variables
	archaeopteryxABI->insert(new RegisterBoundVariable(
		"ctaid_x", , "r0"));

	

	return archaeopteryxABI;
}

}

}

