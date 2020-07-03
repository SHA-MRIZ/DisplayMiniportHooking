#pragma once

#include <ntifs.h>


extern "C" NTSYSAPI NTSTATUS NTAPI ObReferenceObjectByName(
    PUNICODE_STRING ObjectName,
    ULONG Attributes,
    PACCESS_STATE AccessState,
    ACCESS_MASK DesiredAccess,
    POBJECT_TYPE ObjectType,
    KPROCESSOR_MODE AccessMode,
    PVOID ParseContext OPTIONAL,
    PVOID * Object);

extern "C" POBJECT_TYPE * IoDriverObjectType;

/**
 *  Guard a object.
 *  release object when exiting the current context.
 */
template <class T>
class ObjectGuard
{
public:
    ObjectGuard(T object) : _object(object), _isValid(true) {}

    ObjectGuard() : _object(nullptr), _isValid(false) {}

    ~ObjectGuard()
    {
        _isValid = false;
    }

    T& get() { return _object; }
    bool isValid() const { return _isValid; }

protected:
    T _object;
    bool _isValid;
};

class DriverObjectGuard : ObjectGuard<PDRIVER_OBJECT>
{
public:
    DriverObjectGuard(PUNICODE_STRING driverName) : ObjectGuard()
    {
        PDRIVER_OBJECT drvObject = nullptr;

        if (NT_SUCCESS(ObReferenceObjectByName(
            driverName,
            0,
            NULL,
            0,
            *IoDriverObjectType,
            KernelMode,
            NULL,
            reinterpret_cast<PVOID*>(&drvObject))))
        {
            _object = drvObject;
            _isValid = true;
        }
    }

    ~DriverObjectGuard()
    {
        if (isValid() & (_object != nullptr))
        {
            ObDereferenceObject(_object);
            _isValid = false;
        }
    }
};

