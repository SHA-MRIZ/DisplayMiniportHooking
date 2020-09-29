#include "DisplayMiniportHooking.h"


void hookFunction(PVOID* pSourceFunction, PVOID hookFunction)
{
    if (MmIsAddressValid(pSourceFunction))
    {
        InterlockedExchangePointer(pSourceFunction, hookFunction);
    }
}

NTSTATUS hookSetPointerPosition(
    _In_ CONST HANDLE hAdapter,
    _In_ CONST DXGKARG_SETPOINTERPOSITION* setPointerPosition)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(setPointerPosition);

    DbgPrint("Enter to hookSetPointerPosition");

    return reinterpret_cast<PDXGKDDI_SETPOINTERPOSITION>(
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress)(hAdapter, setPointerPosition);
}

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

    return INVALID_OFFSET;
}

PVOID* findCallbackLocationByOffset(void* startAddress, unsigned short offset)
{
    return reinterpret_cast<PVOID*>(
        reinterpret_cast<unsigned short*>(startAddress) + (offset / sizeof(unsigned short)));
}

PVOID* findCallbackLocationInAdapter(void* deviceObjectExtension, void* callback, void* driverExtension)
{
    if ((deviceObjectExtension == nullptr) || (callback == nullptr))
    {
        return nullptr;
    }

    for (unsigned short i = 0; i < LIMIT_DEVICE_OBJECT_EXTENSION / sizeof(unsigned short); i++)
    {
        void* potentialAdapterPointer = reinterpret_cast<void*>(*(reinterpret_cast<PULONG_PTR>(
            reinterpret_cast<unsigned short*>(deviceObjectExtension) + i)));

        if (MmIsAddressValid(potentialAdapterPointer) && (potentialAdapterPointer != driverExtension))
        {
            auto callbackOffset = findOffsetCallback(
                potentialAdapterPointer,
                callback,
                LIMIT_DRIVER_OBJECT_EXTENSION);

            if (callbackOffset != INVALID_OFFSET)
            {
                return findCallbackLocationByOffset(potentialAdapterPointer, callbackOffset);
            }
        }
    }

    return nullptr;
}

void* findCallbackInDriverExtension(
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

    if (callbackOffset == INVALID_OFFSET)
    {
        return nullptr;
    }

    auto callbackLocation = findCallbackLocationByOffset(targetDriverObjectExtension, callbackOffset);

    if (!MmIsAddressValid(callbackLocation))
    {
        return nullptr;
    }

    return reinterpret_cast<void*>(*callbackLocation);
}

VOID Unload(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Unload Driver \r\n");

    hookFunction(
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress);

    delete ORIGINAL_CALLBACKS_INFO;
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
        ORIGINAL_CALLBACKS_INFO = new (NonPagedPool, TAG)WDDM_CALLBACKS_INFO();

        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress = findCallbackInDriverExtension(
            dummyDriverObject.getDriverObjectExtensions(dummyDriverObject.get()),
            targetDriverObject.getDriverObjectExtensions(targetDriverObject.get()),
            dummyInitData.DxgkDdiSetPointerPosition);

        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation = findCallbackLocationInAdapter(
            targetDriverObject.get()->DeviceObject->DeviceExtension,
            ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress,
            targetDriverObject.getDriverObjectExtensions(targetDriverObject.get()));

        if (ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation != nullptr)
        {
            hookFunction(
                ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation,
                hookSetPointerPosition);
        }

        status = unInitializeMiniport(driverObject);

        if (NT_SUCCESS(status))
        {
            driverObject->DriverUnload = Unload;
            driverObject->DriverExtension->AddDevice = nullptr;
        }
    }

    return status;
}