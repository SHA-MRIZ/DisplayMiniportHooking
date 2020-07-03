#include "DisplayMiniportHooking.h"
#include "RAIIUtils.h"


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

    NTSTATUS status = STATUS_SUCCESS;

    DbgPrint("DriverEntry Called \r\n");

    driverObject->DriverUnload = Unload;

    UNICODE_STRING DRV_NAME = RTL_CONSTANT_STRING(L"\\Driver\\SampleDisplay");

    DriverObjectGuard targetDriverObject(&DRV_NAME);

    return status;
}