#pragma once

#include "common.h"
#include "DummyMiniport.h"
#include "RAIIUtils.h"


extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath);

typedef struct _CALLBACK_INFO
{
    void* CallbackAddress;
    PVOID* CallbackLocation;
}CALLBACK_INFO, *PCALLBACK_INFO;

typedef struct _WDDM_CALLBACKS_INFO
{
    CALLBACK_INFO AddDevice;
    CALLBACK_INFO StartDevice;
    CALLBACK_INFO StopDevice;
    CALLBACK_INFO RemoveDevice;
    CALLBACK_INFO DispatchIoRequest;
    CALLBACK_INFO InterruptRoutine;
    CALLBACK_INFO DpcRoutine;
    CALLBACK_INFO QueryChildRelations;
    CALLBACK_INFO QueryChildStatus;
    CALLBACK_INFO QueryDeviceDescriptor;
    CALLBACK_INFO SetPowerState;
    CALLBACK_INFO NotifyAcpiEvent;
    CALLBACK_INFO ResetDevice;
    CALLBACK_INFO Unload;
    CALLBACK_INFO QueryInterface;
    CALLBACK_INFO ControlEtwLogging;
    CALLBACK_INFO QueryAdapterInfo;
    CALLBACK_INFO CreateDevice;
    CALLBACK_INFO CreateAllocation;
    CALLBACK_INFO DestroyAllocation;
    CALLBACK_INFO DescribeAllocation;
    CALLBACK_INFO GetStandardAllocationDriverData;
    CALLBACK_INFO AcquireSwizzlingRange;
    CALLBACK_INFO ReleaseSwizzlingRange;
    CALLBACK_INFO Patch;
    CALLBACK_INFO SubmitCommand;
    CALLBACK_INFO PreemptCommand;
    CALLBACK_INFO BuildPagingBuffer;
    CALLBACK_INFO SetPalette;
    CALLBACK_INFO SetPointerPosition;
    CALLBACK_INFO SetPointerShape;
    CALLBACK_INFO ResetFromTimeout;
    CALLBACK_INFO RestartFromTimeout;
    CALLBACK_INFO Escape;
    CALLBACK_INFO CollectDbgInfo;
    CALLBACK_INFO QueryCurrentFence;
    CALLBACK_INFO IsSupportedVidPn;
    CALLBACK_INFO RecommendFunctionalVidPn;
    CALLBACK_INFO EnumVidPnCofuncModality;
    CALLBACK_INFO SetVidPnSourceAddress;
    CALLBACK_INFO SetVidPnSourceVisibility;
    CALLBACK_INFO CommitVidPn;
    CALLBACK_INFO UpdateActiveVidPnPresentPath;
    CALLBACK_INFO RecommendMonitorModes;
    CALLBACK_INFO RecommendVidPnTopology;
    CALLBACK_INFO GetScanLine;
    CALLBACK_INFO StopCapture;
    CALLBACK_INFO ControlInterrupt;
    CALLBACK_INFO CreateOverlay;
    CALLBACK_INFO DestroyDevice;
    CALLBACK_INFO OpenAllocation;
    CALLBACK_INFO CloseAllocation;
    CALLBACK_INFO Render;
    CALLBACK_INFO Present;
    CALLBACK_INFO UpdateOverlay;
    CALLBACK_INFO FlipOverlay;
    CALLBACK_INFO DestroyOverlay;
    CALLBACK_INFO CreateContext;
    CALLBACK_INFO DestroyContext;
    CALLBACK_INFO LinkDevice;
    CALLBACK_INFO SetDisplayPrivateDriverFormat;
} WDDM_CALLBACKS_INFO, *PWDDM_CALLBACKS_INFO;

constexpr unsigned short LIMIT_DRIVER_OBJECT_EXTENSION = 0x558;
constexpr unsigned short LIMIT_DEVICE_OBJECT_EXTENSION = 0X100;
constexpr unsigned short INVALID_OFFSET = 0xffff;

PWDDM_CALLBACKS_INFO ORIGINAL_CALLBACKS_INFO;