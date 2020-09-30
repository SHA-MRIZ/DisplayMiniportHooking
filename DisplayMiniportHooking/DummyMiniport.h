#pragma once

#include "common.h"

extern "C"
{
    #include <dispmprt.h>
};


NTSTATUS dummyRender(IN_CONST_HANDLE hContext, INOUT_PDXGKARG_RENDER render)
{
    UNREFERENCED_PARAMETER(hContext);
    UNREFERENCED_PARAMETER(render);

    DbgPrint("Enter to dummyRender");

    return STATUS_SUCCESS;
}

NTSTATUS dummyPresent(_In_ CONST HANDLE hAdapter, INOUT_PDXGKARG_PRESENT present)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(present);

    DbgPrint("Enter to dummyPresent");

    return STATUS_SUCCESS;
}

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

    DbgPrint("Enter to dummyDdiStartDevice");

    return STATUS_SUCCESS;
}

NTSTATUS dummyDdiAddDevice(_In_ DEVICE_OBJECT* physicalDeviceObject, _Outptr_ PVOID* deviceContext)
{
    UNREFERENCED_PARAMETER(physicalDeviceObject);
    UNREFERENCED_PARAMETER(deviceContext);

    DbgPrint("Enter to dummyDdiAddDevice");

    return STATUS_SUCCESS;
}

NTSTATUS dummyStopDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    DbgPrint("Enter to dummyStopDevice");

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

    DbgPrint("Enter to dummyDispatchIoRequest");

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

    DbgPrint("Enter to dummySetPowerState");

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

    DbgPrint("Enter to dummyQueryChildRelations");

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

    DbgPrint("Enter to dummyQueryChildStatus");

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

    DbgPrint("Enter to dummyQueryDeviceDescriptor");

    return STATUS_SUCCESS;
}

NTSTATUS APIENTRY dummyQueryAdapterInfo(
    _In_ CONST HANDLE hAdapter,
    _In_ CONST DXGKARG_QUERYADAPTERINFO* queryAdapterInfo)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(queryAdapterInfo);

    DbgPrint("Enter to dummyQueryAdapterInfo");

    return STATUS_SUCCESS;
}

NTSTATUS dummyRemoveDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    DbgPrint("Enter to dummyRemoveDevice");

    return STATUS_SUCCESS;
}

VOID dummyDpcRoutine(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    DbgPrint("Enter to dummyDpcRoutine");
}

BOOLEAN dummyInterruptRoutine(_In_ VOID* deviceContext, _In_ ULONG messageNumber)
{
    UNREFERENCED_PARAMETER(deviceContext);
    UNREFERENCED_PARAMETER(messageNumber);

    DbgPrint("Enter to dummyDpcRoutine");

    return TRUE;
}

VOID dummyUnload()
{
    DbgPrint("Enter to dummyUnload");
}

VOID dummyResetDevice(_In_ VOID* deviceContext)
{
    UNREFERENCED_PARAMETER(deviceContext);

    DbgPrint("Enter to dummyResetDevice");
}

NTSTATUS dummyCreateAllocation(IN_CONST_HANDLE hAdapter, INOUT_PDXGKARG_CREATEALLOCATION createAllocation)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(createAllocation);

    DbgPrint("Enter to dummyCreateAllocation");

    return STATUS_SUCCESS;
}

NTSTATUS dummySetPointerShape(
    _In_ CONST HANDLE hAdapter,
    _In_ CONST DXGKARG_SETPOINTERSHAPE* setPointerShape)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(setPointerShape);

    DbgPrint("Enter to dummySetPointerShape");

    return STATUS_SUCCESS;
}

NTSTATUS dummySetPointerPosition(
    _In_ CONST HANDLE                      hAdapter,
    _In_ CONST DXGKARG_SETPOINTERPOSITION* setPointerPosition)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(setPointerPosition);

    DbgPrint("Enter to dummySetPointerPosition");

    return STATUS_SUCCESS;
}

NTSTATUS dummyCollectdbginfo(
    IN_CONST_HANDLE hAdapter,
    IN_CONST_PDXGKARG_COLLECTDBGINFO collectDbgInfo)
{
    UNREFERENCED_PARAMETER(hAdapter);
    UNREFERENCED_PARAMETER(collectDbgInfo);

    DbgPrint("Enter to dummyCollectdbginfo");

    return STATUS_SUCCESS;
}

NTSTATUS initDisplay(
    PDRIVER_OBJECT driverObject,
    PUNICODE_STRING registryPath,
    PDRIVER_INITIALIZATION_DATA dummyInitData)
{
    if (!ARGUMENT_PRESENT(driverObject) ||
        !ARGUMENT_PRESENT(registryPath) ||
        !ARGUMENT_PRESENT(dummyInitData))
    {
        return STATUS_INVALID_PARAMETER;
    }

    dummyInitData->Version = DXGKDDI_INTERFACE_VERSION;
    dummyInitData->DxgkDdiAddDevice = dummyDdiAddDevice;
    dummyInitData->DxgkDdiStartDevice = dummyDdiStartDevice;
    dummyInitData->DxgkDdiStopDevice = dummyStopDevice;
    dummyInitData->DxgkDdiRemoveDevice = dummyRemoveDevice;
    dummyInitData->DxgkDdiDispatchIoRequest = dummyDispatchIoRequest;
    dummyInitData->DxgkDdiInterruptRoutine = dummyInterruptRoutine;
    dummyInitData->DxgkDdiDpcRoutine = dummyDpcRoutine;
    dummyInitData->DxgkDdiQueryChildRelations = dummyQueryChildRelations;
    dummyInitData->DxgkDdiQueryChildStatus = dummyQueryChildStatus;
    dummyInitData->DxgkDdiQueryDeviceDescriptor = dummyQueryDeviceDescriptor;
    dummyInitData->DxgkDdiSetPowerState = dummySetPowerState;
    dummyInitData->DxgkDdiResetDevice = dummyResetDevice;
    dummyInitData->DxgkDdiUnload = dummyUnload;
    dummyInitData->DxgkDdiSetPointerShape = dummySetPointerShape;
    dummyInitData->DxgkDdiSetPointerPosition = dummySetPointerPosition;
    dummyInitData->DxgkDdiCreateAllocation = dummyCreateAllocation;
    dummyInitData->DxgkDdiCollectDbgInfo = dummyCollectdbginfo;
    dummyInitData->DxgkDdiPresent = dummyPresent;
    dummyInitData->DxgkDdiRender = dummyRender;

    return DxgkInitialize(driverObject, registryPath, dummyInitData);
}

NTSTATUS unInitializeMiniport(PDRIVER_OBJECT driverObject)
{
    return DxgkUnInitialize(driverObject);
}

