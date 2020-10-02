#include "Hooking.h"


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
    DbgPrint("Enter to hookSetPointerPosition \n");

    return reinterpret_cast<PDXGKDDI_SETPOINTERPOSITION>(
        ORIGINAL_CALLBACKS_INFO->SetPointerPosition.CallbackAddress)(hAdapter, setPointerPosition);
}

NTSTATUS hookPresent(
    _In_ CONST HANDLE hAdapter,
    INOUT_PDXGKARG_PRESENT present)
{
    DbgPrint("Enter to hookPresent \n");

    return reinterpret_cast<PDXGKDDI_PRESENT>(
        ORIGINAL_CALLBACKS_INFO->Present.CallbackAddress)(hAdapter, present);
}

NTSTATUS hookRender(
    IN_CONST_HANDLE hContext,
    INOUT_PDXGKARG_RENDER render)
{
    DbgPrint("Enter to hookRender \n");

    return reinterpret_cast<PDXGKDDI_RENDER>(
        ORIGINAL_CALLBACKS_INFO->Render.CallbackAddress)(hContext, render);
}

NTSTATUS hookSubmitCommand(IN_CONST_HANDLE hAdapter, IN_CONST_PDXGKARG_SUBMITCOMMAND submitCommand)
{
    DbgPrint("Enter to hookSubmitCommand \n");

    return reinterpret_cast<PDXGKDDI_SUBMITCOMMAND>(
        ORIGINAL_CALLBACKS_INFO->SubmitCommand.CallbackAddress)(hAdapter, submitCommand);
}

NTSTATUS hookCreateAllocation(IN_CONST_HANDLE hAdapter, INOUT_PDXGKARG_CREATEALLOCATION createAllocation)
{
    DbgPrint("Enter to hookCreateAllocation \n");

    return reinterpret_cast<PDXGKDDI_CREATEALLOCATION>(
        ORIGINAL_CALLBACKS_INFO->CreateAllocation.CallbackAddress)(hAdapter, createAllocation);
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

        auto driverExtensionWithLimit = reinterpret_cast<void*>(
            reinterpret_cast<unsigned short*>(driverExtension) + LIMIT_DRIVER_OBJECT_EXTENSION);

        if (MmIsAddressValid(potentialAdapterPointer) &&
           ((potentialAdapterPointer < driverExtension) ||
            (potentialAdapterPointer > driverExtensionWithLimit)))
        {
            auto callbackOffset = findOffsetCallback(
                potentialAdapterPointer,
                callback,
                limitDriverObjectExtension);

            if (callbackOffset != INVALID_OFFSET)
            {
                DbgPrint("Adapter: %p \n", potentialAdapterPointer);
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

    hookFunction(
        ORIGINAL_CALLBACKS_INFO->SubmitCommand.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->SubmitCommand.CallbackAddress);

    hookFunction(
        ORIGINAL_CALLBACKS_INFO->CreateAllocation.CallbackLocation,
        ORIGINAL_CALLBACKS_INFO->CreateAllocation.CallbackAddress);

    delete ORIGINAL_CALLBACKS_INFO;
}

void hookCallbacks(
    void* dummyDriverObjectExtension,
    void* targetDriverObjectExtension,
    void* targetDeviceObjectExtension,
    DRIVER_INITIALIZATION_DATA& dummyInitData)
{
    if ((dummyDriverObjectExtension == nullptr) ||
        (targetDriverObjectExtension == nullptr) ||
        (targetDeviceObjectExtension == nullptr))
    {
        return;
    }

    DbgPrint("Target Driver Object Extension: %p \n", targetDriverObjectExtension);
    DbgPrint("Target Device Object Extension: %p \n", targetDeviceObjectExtension);

    ORIGINAL_CALLBACKS_INFO = new (NonPagedPool, TAG)WDDM_CALLBACKS_INFO();

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

    // Submit Command
    hookCallbackInAdapter(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        targetDeviceObjectExtension,
        dummyInitData.DxgkDdiSubmitCommand,
        hookSubmitCommand,
        &ORIGINAL_CALLBACKS_INFO->SubmitCommand.CallbackAddress,
        reinterpret_cast<PVOID*>(&ORIGINAL_CALLBACKS_INFO->SubmitCommand.CallbackLocation));

    // Create Allocation
    hookCallbackInAdapter(
        dummyDriverObjectExtension,
        targetDriverObjectExtension,
        targetDeviceObjectExtension,
        dummyInitData.DxgkDdiCreateAllocation,
        hookCreateAllocation,
        &ORIGINAL_CALLBACKS_INFO->CreateAllocation.CallbackAddress,
        reinterpret_cast<PVOID*>(&ORIGINAL_CALLBACKS_INFO->CreateAllocation.CallbackLocation));
}
