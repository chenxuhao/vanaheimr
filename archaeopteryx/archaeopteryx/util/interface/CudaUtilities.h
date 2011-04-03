
/*! \file   CudaUtilities.h
	\date   Saturday Feburary 26, 2011
	\author Gregory Diamos and Sudnya Diamos
		<gregory.diamos@gatech.edu, mailsudnya@gmail.com>
	\brief  A set of common CUDA functions.
*/

#pragma once

// Standard Library Includes
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <cstdio>

/*! \brief Common utility functions */
namespace util
{

inline __host__ __device__ unsigned int align(unsigned int address,
    unsigned int alignment)
{
    unsigned int remainder = address % alignment;
    return remainder == 0 ? address : (address + alignment - remainder);
}

template<typename T>
__device__ T getParameter(void* parameter, unsigned int byte = 0)
{
	return *(T*)((char*)parameter + byte);
}

inline void check(cudaError_t status)
{
    if(status != cudaSuccess)
    {
        std::cerr << cudaGetErrorString(status) << std::endl;
        std::abort();
    }
}

const unsigned int hostBufferSize = 4096;

typedef void (*AsyncFunctionPointer)(void*);

class AsyncFunctionRecord
{
public:
	const char* name;
	AsyncFunctionPointer function;
};

__device__ void* hostBuffer;
__device__ AsyncFunctionRecord functionTable[] = {
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 },
		{ 0, 0 }
		};

__device__ unsigned int strlen(const char* s)
{
	const char* i = s;
	while(*i) ++i;
	
	return i - s;
}

__device__ inline void async_system_call(const char* name,
	void* p1 = 0, void* p2 = 0, void* p3 = 0)
{
    printf("async_system_call(%s)\n", name);

	unsigned int* offset = (unsigned int*)hostBuffer;
	char* dataBase       = (char*)        hostBuffer;
	
	unsigned int packetSizeOffset = *offset;
	
	if(packetSizeOffset == 0)
	{
	    packetSizeOffset = sizeof(unsigned int);
	}
	
	unsigned int startingOffset = packetSizeOffset + sizeof(unsigned int);

	unsigned int size = strlen(name) + 1;

	std::memcpy(dataBase + startingOffset, name, size);
	unsigned int currentOffset = startingOffset + size;

    currentOffset = align(currentOffset, sizeof(void*));

	if(p1 != 0)
	{
		std::memcpy(dataBase + currentOffset, &p1, sizeof(void*));
		currentOffset += sizeof(void*);
	}

	if(p2 != 0)
	{
		std::memcpy(dataBase + currentOffset, &p2, sizeof(void*));
		currentOffset += sizeof(void*);
	}

	if(p3 != 0)
	{
		std::memcpy(dataBase + currentOffset, &p3, sizeof(void*));
		currentOffset += sizeof(void*);
	}
	
	*offset = currentOffset;
	
	size = currentOffset - startingOffset + sizeof(unsigned int);

    printf(" packet offset: %d\n", packetSizeOffset);
	printf(" packet size:   %d\n", size);
	
	std::memcpy(dataBase + packetSizeOffset, &size, sizeof(unsigned int));
}

inline void setupHostReflection()
{
	unsigned int* buffer = 0;
	check(cudaHostAlloc(&buffer, hostBufferSize, cudaHostAllocDefault));
	
	*buffer = 0;
	
	check(cudaMemcpyToSymbol(hostBuffer, &buffer, sizeof(void*)));
}

__device__ int strcmp(const char* str1, const char* str2)
{
	while(*str1 && *str2)
	{
		if(*str1 != *str2) return 1;
		
		++str1;
		++str2;
	}
	
	if(!*str1 && !*str2) return 0;
	
	return 1;
}

__global__ void dispatch(const char* name, void* payload)
{
    printf("dispatch(%s)\n", name);
	for(AsyncFunctionRecord* record = functionTable;
		record->name != 0; ++record)
	{
		if(strcmp(record->name, name) == 0)
		{
            printf(" dispatch-launch(%s)\n", name);
			record->function(payload);
		}
	}
}

inline void teardownHostReflection()
{
	void* localhostBuffer = 0;
	check(cudaMemcpyFromSymbol(&localhostBuffer, hostBuffer, sizeof(void*)));

	unsigned int* hostBufferBase = (unsigned int*)localhostBuffer;
	unsigned int totalSize  = *hostBufferBase;
	unsigned int packetSize = 0;
	
	char* dataBase = (char*)(hostBufferBase);
	
	for(unsigned int i = sizeof(unsigned int); i < totalSize; i += packetSize)
	{
		packetSize = *(unsigned int*)(dataBase + i);
		
		unsigned int nameOffset = i + sizeof(unsigned int);
		const char* name = dataBase + nameOffset;
        printf("Found packet of size %d at %d\n", packetSize, i);
		printf("Launching async function %s\n", name);
		
		unsigned int payloadOffset = align(nameOffset + std::strlen(name) + 1,
		    sizeof(void*));
		printf(" payload offset %d\n", payloadOffset);
		
		const char* payload = dataBase + payloadOffset;
		
		dispatch<<<1, 1, 1>>>(name, (void*)payload);
		check(cudaThreadSynchronize());
	}
	
	check(cudaFreeHost(localhostBuffer));
}

}

