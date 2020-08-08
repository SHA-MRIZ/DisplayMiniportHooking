#include "DisplayMiniportHooking.h"


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

void* findCallbackByOffset(void* startAddress, unsigned short offset)
{
    return reinterpret_cast<void*>(*(reinterpret_cast<PULONG_PTR>(
        reinterpret_cast<unsigned short*>(startAddress) + (offset / sizeof(unsigned short)))));
}

void* findCallbackByDummyCallback(
    void* dummyDriverObjectExtension,
    void* targetDriverObjectExtension,
    void* dummyCallback)
{
    if ((dummyDriverObjectExtension == nullptr) ||
        (targetDriverObjectExtension == nullptr) ||
        (dummyCallback == nullptr))
    {
        return nullptr;
    }

    auto callbackOffset = findOffsetCallback(
        dummyDriverObjectExtension,
        dummyCallback,
        LIMIT_DRIVER_OBJECT_EXTENSION);

    return findCallbackByOffset(targetDriverObjectExtension, callbackOffset);
}

void findWddmCallbacksOffsets(
    DriverObjectGuard& dummyWddm,
    DriverObjectGuard& targetWddm,
    DRIVER_INITIALIZATION_DATA& dummyInitData,
    DRIVER_INITIALIZATION_DATA& targetDriverInitData)
{
    auto dummyDriverObjectExtension = dummyWddm.getDriverObjectExtensions(dummyWddm.get());
    auto targetDriverObjectExtension = targetWddm.getDriverObjectExtensions(targetWddm.get());

    targetDriverInitData.DxgkDdiAddDevice = reinterpret_cast<PDXGKDDI_ADD_DEVICE>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiAddDevice)));

    targetDriverInitData.DxgkDdiDpcRoutine = reinterpret_cast<PDXGKDDI_DPC_ROUTINE>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiDpcRoutine)));

    targetDriverInitData.DxgkDdiQueryChildRelations = reinterpret_cast<PDXGKDDI_QUERY_CHILD_RELATIONS>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiQueryChildRelations)));

    targetDriverInitData.DxgkDdiInterruptRoutine = reinterpret_cast<PDXGKDDI_INTERRUPT_ROUTINE>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiInterruptRoutine)));

    targetDriverInitData.DxgkDdiSetPowerState = reinterpret_cast<PDXGKDDI_SET_POWER_STATE>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiSetPowerState)));

    targetDriverInitData.DxgkDdiCreateAllocation = reinterpret_cast<PDXGKDDI_CREATEALLOCATION>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiCreateAllocation)));

    targetDriverInitData.DxgkDdiSetPointerShape = reinterpret_cast<PDXGKDDI_SETPOINTERSHAPE>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiSetPointerShape)));

    targetDriverInitData.DxgkDdiCollectDbgInfo = reinterpret_cast<PDXGKDDI_COLLECTDBGINFO>(
        findCallbackByDummyCallback(
            dummyDriverObjectExtension,
            targetDriverObjectExtension,
            reinterpret_cast<void*>(dummyInitData.DxgkDdiCollectDbgInfo)));
}

void HookCallbacks(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);
}

VOID Unload(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Unload Driver \r\n");

    delete originalCallbacks;
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
        DriverObjectGuard dummyDriverObject(driverObject);
        originalCallbacks = new (NonPagedPool, TAG)DRIVER_INITIALIZATION_DATA();

        findWddmCallbacksOffsets(dummyDriverObject, targetDriverObject, dummyInitData, *originalCallbacks);

        status = unInitializeMiniport(driverObject);

        if (NT_SUCCESS(status))
        {
            driverObject->DriverUnload = Unload;
            driverObject->DriverExtension->AddDevice = nullptr;
        }
    }

    return status;
}