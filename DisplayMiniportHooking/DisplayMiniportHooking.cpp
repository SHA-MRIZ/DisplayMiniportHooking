#include "DisplayMiniportHooking.h"
#include "DummyMiniport.h"
#include "RAIIUtils.h"


unsigned short findOffsetCallback(void* startAddress, void* callback, unsigned short limit)
{
    for (unsigned short i = 0; i < limit / sizeof(unsigned short); i++)
    {
        void* potentialPointer = reinterpret_cast<void*>(*(reinterpret_cast<PULONG_PTR>(
            reinterpret_cast<unsigned short*>(startAddress) + i)));

        if (callback == potentialPointer)
        {
            return i * sizeof(unsigned short);
        }
    }

    return 0;
}

void HookCallbacks(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);
}

VOID Unload(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Unload Driver \r\n");
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    DbgPrint("DriverEntry Called \r\n");

    UNICODE_STRING DRV_NAME = RTL_CONSTANT_STRING(L"\\Driver\\SampleDisplay");

    DriverObjectGuard targetDriverObject(&DRV_NAME);

    NTSTATUS status = initDisplay(driverObject, registryPath);

    if (NT_SUCCESS(status))
    {
        status = unInitializeMiniport(driverObject);
        if (NT_SUCCESS(status))
        {
            driverObject->DriverUnload = Unload;
            driverObject->DriverExtension->AddDevice = nullptr;
        }
    }

    return status;
}