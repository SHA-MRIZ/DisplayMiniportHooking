#pragma once

extern "C"
{
    #include <ntifs.h>
    #include <dispmprt.h>
};


NTSTATUS dummyDdiStartDevice(
    _In_  VOID* deviceContext,
    _In_  DXGK_START_INFO* dxgkStartInfo,
    _In_  DXGKRNL_INTERFACE* dxgkInterface,
    _Out_ ULONG* numberOfViews,
    _Out_ ULONG* numberOfChildren)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(dxgkStartInfo);
    UNREFERENCED_PARAMETER(dxgkInterface);
    UNREFERENCED_PARAMETER(numberOfViews);
    UNREFERENCED_PARAMETER(numberOfChildren);

    return STATUS_SUCCESS;
}

NTSTATUS dummyDdiAddDevice(_In_ DEVICE_OBJECT* physicalDeviceObject, _Outptr_ PVOID* deviceContext)
{
    UNREFERENCED_PARAMETER(physicalDeviceObject);
    UNREFERENCED_PARAMETER(deviceContext);

    return STATUS_SUCCESS;
}

NTSTATUS dummyStopDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    return STATUS_SUCCESS;
}

NTSTATUS dummyDispatchIoRequest(
    _In_ VOID* deviceContext,
    _In_ ULONG vidPnSourceId,
    _In_ VIDEO_REQUEST_PACKET* videoRequestPacket)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(vidPnSourceId);
    UNREFERENCED_PARAMETER(videoRequestPacket);

    return STATUS_SUCCESS;
}

NTSTATUS dummySetPowerState(
    _In_ VOID* deviceContext,
    _In_ ULONG hardwareUid,
    _In_ DEVICE_POWER_STATE devicePowerState,
    _In_ POWER_ACTION actionType)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(hardwareUid);
    UNREFERENCED_PARAMETER(devicePowerState);
    UNREFERENCED_PARAMETER(actionType);

    return STATUS_SUCCESS;
}

NTSTATUS dummyQueryChildRelations(
    _In_ VOID* deviceContext,
    _Out_writes_bytes_(ChildRelationsSize) DXGK_CHILD_DESCRIPTOR* childRelations,
    _In_ ULONG childRelationsSize)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(childRelations);
    UNREFERENCED_PARAMETER(childRelationsSize);

    return STATUS_SUCCESS;
}

NTSTATUS dummyQueryChildStatus(
    _In_ VOID* deviceContext,
    _Inout_ DXGK_CHILD_STATUS* childStatus,
    _In_ BOOLEAN nonDestructiveOnly)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(childStatus);
    UNREFERENCED_PARAMETER(nonDestructiveOnly);

    return STATUS_SUCCESS;
}

NTSTATUS dummyQueryDeviceDescriptor(
    _In_  VOID* deviceContext,
    _In_  ULONG childUid,
    _Inout_ DXGK_DEVICE_DESCRIPTOR* deviceDescriptor)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(childUid);
    UNREFERENCED_PARAMETER(deviceDescriptor);

    return STATUS_SUCCESS;
}

NTSTATUS APIENTRY dummyQueryAdapterInfo(
    _In_ CONST HANDLE hAdapter,
    _In_ CONST DXGKARG_QUERYADAPTERINFO* queryAdapterInfo)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(queryAdapterInfo);

    return STATUS_SUCCESS;
}

NTSTATUS dummyRemoveDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    return STATUS_SUCCESS;
}

VOID dummyDpcRoutine(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);
}

BOOLEAN dummyInterruptRoutine(_In_ VOID* deviceContext, _In_ ULONG messageNumber)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(messageNumber);
    return TRUE;
}

VOID dummyUnload()
{
}

VOID dummyResetDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);
}

NTSTATUS initDisplay(PDRIVER_OBJECT driverObject, PUNICODE_STRING registryPath)
{
    if (!ARGUMENT_PRESENT(driverObject) || !ARGUMENT_PRESENT(registryPath))
    {
        return STATUS_INVALID_PARAMETER;
    }

    DRIVER_INITIALIZATION_DATA driverInitializationData = { '\0' };

    driverInitializationData.Version = DXGKDDI_INTERFACE_VERSION;

    driverInitializationData.DxgkDdiAddDevice = dummyDdiAddDevice;
    driverInitializationData.DxgkDdiStartDevice = dummyDdiStartDevice;
    driverInitializationData.DxgkDdiStopDevice = dummyStopDevice;
    driverInitializationData.DxgkDdiRemoveDevice = dummyRemoveDevice;
    driverInitializationData.DxgkDdiDispatchIoRequest = dummyDispatchIoRequest;
    driverInitializationData.DxgkDdiInterruptRoutine = dummyInterruptRoutine;
    driverInitializationData.DxgkDdiDpcRoutine = dummyDpcRoutine;
    driverInitializationData.DxgkDdiQueryChildRelations = dummyQueryChildRelations;
    driverInitializationData.DxgkDdiQueryChildStatus = dummyQueryChildStatus;
    driverInitializationData.DxgkDdiQueryDeviceDescriptor = dummyQueryDeviceDescriptor;
    driverInitializationData.DxgkDdiSetPowerState = dummySetPowerState;
    driverInitializationData.DxgkDdiResetDevice = dummyResetDevice;
    driverInitializationData.DxgkDdiUnload = dummyUnload;

    return DxgkInitialize(driverObject, registryPath, &driverInitializationData);
}

NTSTATUS unInitializeMiniport(PDRIVER_OBJECT driverObject)
{
    return DxgkUnInitialize(driverObject);
}

