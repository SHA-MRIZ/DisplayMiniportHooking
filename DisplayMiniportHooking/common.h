#pragma once

#include "ntddk.h"
#include "RAIIUtils.h"

extern "C"
{
#include <dispmprt.h>
};


constexpr ULONG TAG = 'DMH';

void* __cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag = 0);

void __cdecl operator delete(void* p, unsigned __int64);

NTSTATUS readMemorySafe(void* targetAddress, void* buffer, size_t length);