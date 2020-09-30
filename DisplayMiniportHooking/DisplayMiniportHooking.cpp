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

NTSTATUS hookPresent(
    _In_ CONST HANDLE hAdapter,
    INOUT_PDXGKARG_PRESENT present)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(present);

    DbgPrint("Enter to hookPresent");

    return reinterpret_cast<PDXGKDDI_PRESENT>(
        ORIGINAL_CALLBACKS_INFO->Present.CallbackAddress)(hAdapter, present);
}

NTSTATUS hookRender(
    IN_CONST_HANDLE hContext,
    INOUT_PDXGKARG_RENDER render)
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(render);

    DbgPrint("Enter to hookRender");

    return reinterpret_cast<PDXGKDDI_RENDER>(
        ORIGINAL_CALLBACKS_INFO->Render.CallbackAddress)(hContext, render);
}

unsigned short findOffsetCallback(void* startAddress, void* callback, unsigned short limit)
{
    ArrayGuard<unsigned short> memoryBuffer;
    memoryBuffer.allocate(NonPagedPool, limit / sizeof(unsigned short));

    if (!NT_SUCCESS(readMemorySafe(startAddress, memoryBuffer.get(), limit)))
    {
        return INVALID_OFFSET;
    }

    for (unsigned short i = 0; i < limit / sizeof(unsigned short); i++)
    {
        void* potentialPointer = reinterpret_cast<void*>(*(reinterpret_cast<PULONG_PTR>(
            reinterpret_cast<unsigned short*>(memoryBuffer.get()) + i)));

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

PVOID* findCallbackLocationInAdapter(
    void* deviceObjectExtension,
    unsigned short limitDeviceObjectExtension,
    void* driverExtension,
    unsigned short limitDriverObjectExtension,
    void* callback)
{
    if ((deviceObjectExtension == nullptr) || (callback == nullptr))
    {
        return nullptr;
    }

    ArrayGuard<unsigned short> deviceExtensionMemoryBuffer;
    deviceExtensionMemoryBuffer.allocate(NonPagedPool, limitDeviceObjectExtension / sizeof(unsigned short));

    if (!NT_SUCCESS(readMemorySafe(
        deviceObjectExtension,
        deviceExtensionMemoryBuffer.get(),
        limitDeviceObjectExtension)))
    {
        return nullptr;
    }

    for (unsigned short i = 0; i < limitDeviceObjectExtension / sizeof(unsigned short); i++)
    {
        void* potentialAdapterPointer = reinterpret_cast<void*>(*(reinterpret_cast<PULONG_PTR>(
            reinterpret_cast<unsigned short*>(deviceExtensionMemoryBuffer.get()) + i)));

        if (MmIsAddressValid(potentialAdapterPointer) && (potentialAdapterPointer != driverExtension))
        {
            auto callbackOffset = findOffsetCallback(
                potentialAdapterPointer,
                callback,
                limitDriverObjectExtension);

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

void hookCallbackInAdapter(
    void* dummyDriverObjectExtension,
    void* targetDriverObjectExtension,
    void* targetDeviceObjectExtension,
    void* dummyCallback,
    void* hookCallback,
    PVOID* callbackAddress,
    PVOID* callbackLocation)
{
    *callbackAddress = findCallbackInDriverExtension(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        dummyCallback);

    *callbackLocation = findCallbackLocationInAdapter(
        targetDeviceObjectExtension,
        LIMIT_DEVICE_OBJECT_EXTENSION,
        targetDriverObjectExtension,
        LIMIT_DRIVER_OBJECT_EXTENSION,
        *callbackAddress);

    if ((*callbackAddress != nullptr) && (*callbackLocation != nullptr))
    {
        hookFunction(reinterpret_cast<PVOID*>(*callbackLocation), hookCallback);
    }
}

void unHookCallbacks()
{
    hookFunction(
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress);

    hookFunction(
        ORIGINAL_CALLBACKS_INFO->Present.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->Present.CallbackAddress);

    hookFunction(
        ORIGINAL_CALLBACKS_INFO->Render.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->Render.CallbackAddress);
}

void hookCallbacks(
    void* dummyDriverObjectExtension,
    void* targetDriverObjectExtension,
    void* targetDeviceObjectExtension,
    DRIVER_INITIALIZATION_DATA dummyInitData)
{
    // Set Pointer Position
    hookCallbackInAdapter(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        targetDeviceObjectExtension,
        dummyInitData.DxgkDdiSetPointerPosition,
        hookSetPointerPosition,
        &ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress,
        reinterpret_cast<PVOID*>(&ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackLocation));

    // Present
    hookCallbackInAdapter(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        targetDeviceObjectExtension,
        dummyInitData.DxgkDdiPresent,
        hookPresent,
        &ORIGINAL_CALLBACKS_INFO->Present.CallbackAddress,
        reinterpret_cast<PVOID*>(&ORIGINAL_CALLBACKS_INFO->Present.CallbackLocation));

    // Render
    hookCallbackInAdapter(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        targetDeviceObjectExtension,
        dummyInitData.DxgkDdiRender,
        hookRender,
        &ORIGINAL_CALLBACKS_INFO->Render.CallbackAddress,
        reinterpret_cast<PVOID*>(&ORIGINAL_CALLBACKS_INFO->Render.CallbackLocation));
}

VOID Unload(PDRIVER_OBJECT driverObject)
{
    UNREFERENCED_PARAMETER(driverObject);

    DbgPrint("Unload Driver \r\n");

    unHookCallbacks();

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


        hookCallbacks(
            dummyDriverObject.getDriverObjectExtension(dummyDriverObject.get()),
            targetDriverObject.getDriverObjectExtension(targetDriverObject.get()),
            targetDriverObject.get()->DeviceObject->DeviceExtension,
            dummyInitData);

        status = unInitializeMiniport(driverObject);

        if (NT_SUCCESS(status))
        {
            driverObject->DriverUnload = Unload;
            driverObject->DriverExtension->AddDevice = nullptr;
        }
    }

    return status;
}