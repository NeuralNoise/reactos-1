/*
 * COPYRIGHT:   See COPYING in the top level directory
 * PROJECT:     ReactOS NDIS library
 * FILE:        ndis/main.c
 * PURPOSE:     Driver entry point
 * PROGRAMMERS: Casper S. Hornstrup (chorns@users.sourceforge.net)
 *              Vizzini (vizzini@plasmic.com)
 * REVISIONS:
 *   CSH 01/08-2000 Created
 *   8/20/2003 Vizzini - NDIS4/5 revisions
 */
#include <ndissys.h>
#include <protocol.h>
#include <miniport.h>


#ifdef DBG

/* See debug.h for debug/trace constants */
DWORD DebugTraceLevel = MIN_TRACE;
//DWORD DebugTraceLevel = -1;

#endif /* DBG */

extern KSPIN_LOCK OrphanAdapterListLock;
extern LIST_ENTRY OrphanAdapterListHead;

VOID MainUnload(
    PDRIVER_OBJECT DriverObject)
/*
 * FUNCTION: Unloads the driver
 * ARGUMENTS:
 *     DriverObject = Pointer to driver object created by the system
 */
{
    NDIS_DbgPrint(MAX_TRACE, ("Leaving.\n"));
}

NTSTATUS
#ifndef _MSC_VER
STDCALL
#endif
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath)
/*
 * FUNCTION: Main driver entry point
 * ARGUMENTS:
 *     DriverObject = Pointer to a driver object for this driver
 *     RegistryPath = Registry node for configuration parameters
 * RETURNS:
 *     Status of driver initialization
 */
{
    NDIS_DbgPrint(MAX_TRACE, ("Called.\n"));

    InitializeListHead(&ProtocolListHead);
    KeInitializeSpinLock(&ProtocolListLock);

    InitializeListHead(&MiniportListHead);
    KeInitializeSpinLock(&MiniportListLock);

    InitializeListHead(&AdapterListHead);
    KeInitializeSpinLock(&AdapterListLock);

    InitializeListHead(&OrphanAdapterListHead);
    KeInitializeSpinLock(&OrphanAdapterListLock);

    DriverObject->DriverUnload = (PDRIVER_UNLOAD)MainUnload;

    /* 
     * until we have PNP support, query the enum key and NdisFindDevice() each one
     * NOTE- this will load and start other services before this one returns STATUS_SUCCESS.
     * I hope there aren't code reentrancy problems. :) 
     */
    /* NdisStartDevices(); */

    return STATUS_SUCCESS;
}

/*
 * @unimplemented
 */
VOID
CDECL
NdisWriteErrorLogEntry(
    IN  NDIS_HANDLE     NdisAdapterHandle,
    IN  NDIS_ERROR_CODE ErrorCode,
    IN  ULONG           NumberOfErrorValues,
    IN  ULONG           ERROR_LOG_MAXIMUM_SIZE)
/*  IN  ULONG           ...) 
 *  ERROR_LOG_MAXIMUM_SIZE = ... in MSDN
 */
/*
 * FUNCTION: Write a syslog error
 * ARGUMENTS:
 *     NdisAdapterHandle:  Handle passed into MiniportInitialize
 *     ErrorCode:  32-bit error code to be logged
 *     NumberOfErrorValues: number of errors to log
 *     Variable: list of log items
 * NOTES:
 *     - THIS IS >CDECL<
 *     - This needs to be fixed to do var args
 */
{
  /*
   * XXX This may be tricky due to the va_arg thing.  I don't
   * want to figure it out now so it's just gonna be disabled.
   */
  NDIS_DbgPrint(MIN_TRACE, ("ERROR: ErrorCode 0x%x\n", ErrorCode));
#if DBG
  /* break into a debugger so we can see what's up */
  __asm__("int $3\n");
#endif
}

/*
 * @implemented
 */
VOID
EXPORT
NdisInitializeReadWriteLock(
    IN  PNDIS_RW_LOCK   Lock)
/*
 * FUNCTION:
 * ARGUMENTS:
 * NOTES:
 *    NDIS 5.0
 */
{
	memset(Lock,0,sizeof(NDIS_RW_LOCK));
}

/*
 * @unimplemented
 */
NDIS_STATUS
EXPORT
NdisWriteEventLogEntry(
    IN  PVOID       LogHandle,
    IN  NDIS_STATUS EventCode,
    IN  ULONG       UniqueEventValue,
    IN  USHORT      NumStrings,
    IN  PVOID       StringsList OPTIONAL,
    IN  ULONG       DataSize,
    IN  PVOID       Data        OPTIONAL)
/*
 * FUNCTION:
 * ARGUMENTS:
 * NOTES:
 *    NDIS 5.0
 */
{
	/*
	 * gonna try just returning true
	 *
    UNIMPLEMENTED

    return NDIS_STATUS_FAILURE;
	 */
    NDIS_DbgPrint(MAX_TRACE, ("Called.\n"));
	return NDIS_STATUS_SUCCESS;
}


/* EOF */
