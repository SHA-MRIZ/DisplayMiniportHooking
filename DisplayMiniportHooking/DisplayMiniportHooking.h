#pragma once

#include "common.h"
#include "DummyMiniport.h"
#include "RAIIUtils.h"


extern "C" DRIVER_INITIALIZE DriverEntry;

extern "C" NTSTATUS DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath);