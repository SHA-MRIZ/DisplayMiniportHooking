#pragma once

#include "common.h"
#include "DummyMiniport.h"
#include "RAIIUtils.h"


extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath);

constexpr unsigned short LIMIT_DRIVER_OBJECT_EXTENSION = 0x558;

PDRIVER_INITIALIZATION_DATA originalCallbacks;