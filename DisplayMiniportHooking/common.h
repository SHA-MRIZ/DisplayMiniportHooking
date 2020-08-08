#pragma once

#include "ntddk.h"


constexpr ULONG TAG = 'DMH';

void* __cdecl operator new(size_t size, POOL_TYPE pool, ULONG tag = 0)
{
    auto pointer = ExAllocatePoolWithTag(pool, size, tag);

    if (pointer != nullptr)
    {
        RtlZeroMemory(pointer, size);
    }

    return pointer;
}

void __cdecl operator delete(void* p, unsigned __int64)
{
    ExFreePool(p);
}