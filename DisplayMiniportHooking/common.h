#pragma once

#include "ntddk.h"
#include "RAIIUtils.h"


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

NTSTATUS readMemorySafe(void* targetAddress, void* buffer, size_t length)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    ArrayGuard<unsigned short> nonPagedBuffer;
    nonPagedBuffer.allocate(NonPagedPool, length / sizeof(unsigned short));

    size_t numberOfBytesTransferred;
    MM_COPY_ADDRESS addrToRead;
    addrToRead.VirtualAddress = targetAddress;

    status = MmCopyMemory(
        nonPagedBuffer.get(),
        addrToRead,
        length,
        MM_COPY_MEMORY_VIRTUAL,
        &numberOfBytesTransferred);

    if (STATUS_SUCCESS == status)
    {
        RtlCopyMemory(buffer, nonPagedBuffer.get(), numberOfBytesTransferred);
    }

    return status;
}