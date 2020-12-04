#include "DisplayMiniportHooking.h"


VOID Unload(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Unload Driver \r\n");

    unHookCallbacks();
}

extern "C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT driverObject, _In_ PUNICODE_STRING registryPath)
{
    UNREFERENCED_PARAMETER(registryPath);

    DRIVER_INITIALIZATION_DATA dummyInitData = { '/0' };
    UNICODE_STRING TARGET_DRV_NAME = RTL_CONSTANT_STRING(L"\\Driver\\BasicRender");

    DbgPrint("DriverEntry Called \r\n");
    DriverObjectGuard targetDriverObject(&TARGET_DRV_NAME);

    if (!targetDriverObject.isValid())
    {
        return STATUS_UNSUCCESSFUL;
    }

    NTSTATUS status = initDisplay(driverObject, registryPath, &dummyInitData);

    if (NT_SUCCESS(status))
    {
        if (targetDriverObject.get()->DeviceObject != nullptr)
        {
            DriverObjectGuard dummyDriverObject(driverObject);

            hookCallbacks(
                dummyDriverObject.getDriverObjectExtension(dummyDriverObject.get()),
                targetDriverObject.getDriverObjectExtension(targetDriverObject.get()),
                targetDriverObject.get()->DeviceObject->DeviceExtension,
                dummyInitData);
        }

        status = unInitializeMiniport(driverObject);

        // Overide the callbacks that DxgkInitialize set to enable unloading of the driver
        if (NT_SUCCESS(status))
        {
            driverObject->DriverUnload = Unload;
            driverObject->DriverExtension->AddDevice = nullptr;
        }
    }

    return status;
}