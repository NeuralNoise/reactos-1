/* $Id: handle.c,v 1.17 2004/10/03 10:05:56 weiden Exp $
 *
 * COPYRIGHT:       See COPYING in the top level directory
 * PROJECT:         ReactOS system libraries
 * FILE:            lib/kernel32/misc/handle.c
 * PURPOSE:         Object  functions
 * PROGRAMMER:      Ariadne ( ariadne@xs4all.nl)
 * UPDATE HISTORY:
 *                  Created 01/11/98
 */

/* INCLUDES ******************************************************************/

#include <k32.h>

#define NDEBUG
#include "../include/debug.h"

/* GLOBALS *******************************************************************/

BOOL STDCALL
InternalGetProcessId (HANDLE hProcess, LPDWORD lpProcessId);

HANDLE STDCALL
DuplicateConsoleHandle (HANDLE	hConsole,
			DWORD   dwDesiredAccess,
			BOOL	bInheritHandle,
			DWORD	dwOptions);

/* FUNCTIONS *****************************************************************/

/*
 * @implemented
 */
BOOL WINAPI
GetHandleInformation (HANDLE hObject,
		      LPDWORD lpdwFlags)
{
  PRTL_USER_PROCESS_PARAMETERS Ppb;
  OBJECT_HANDLE_ATTRIBUTE_INFORMATION HandleInfo;
  ULONG BytesWritten;
  NTSTATUS Status;
  DWORD Flags;
  
  Ppb = NtCurrentPeb()->ProcessParameters;
  switch ((ULONG)hObject)
  {
    case STD_INPUT_HANDLE:
      hObject = Ppb->hStdInput;
      break;
    case STD_OUTPUT_HANDLE:
      hObject = Ppb->hStdOutput;
      break;
    case STD_ERROR_HANDLE:
      hObject = Ppb->hStdError;
      break;
  }

  Status = NtQueryObject (hObject,
			  ObjectHandleInformation,
			  &HandleInfo,
			  sizeof(OBJECT_HANDLE_ATTRIBUTE_INFORMATION),
			  &BytesWritten);
  if (NT_SUCCESS(Status))
  {
    Flags = 0;
    if (HandleInfo.Inherit)
      Flags |= HANDLE_FLAG_INHERIT;
    if (HandleInfo.ProtectFromClose)
      Flags |= HANDLE_FLAG_PROTECT_FROM_CLOSE;

    *lpdwFlags = Flags;

    return TRUE;
  }
  else
  {
    SetLastErrorByStatus (Status);
    return FALSE;
  }
}


/*
 * @implemented
 */
BOOL STDCALL
SetHandleInformation (HANDLE hObject,
		      DWORD dwMask,
		      DWORD dwFlags)
{
  PRTL_USER_PROCESS_PARAMETERS Ppb;
  OBJECT_HANDLE_ATTRIBUTE_INFORMATION HandleInfo;
  ULONG BytesWritten;
  NTSTATUS Status;

  Ppb = NtCurrentPeb()->ProcessParameters;
  switch ((ULONG)hObject)
  {
    case STD_INPUT_HANDLE:
      hObject = Ppb->hStdInput;
      break;
    case STD_OUTPUT_HANDLE:
      hObject = Ppb->hStdOutput;
      break;
    case STD_ERROR_HANDLE:
      hObject = Ppb->hStdError;
      break;
  }

  Status = NtQueryObject (hObject,
			  ObjectHandleInformation,
			  &HandleInfo,
			  sizeof(OBJECT_HANDLE_ATTRIBUTE_INFORMATION),
			  &BytesWritten);
  if (NT_SUCCESS(Status))
  {
    HandleInfo.Inherit = ((dwFlags & HANDLE_FLAG_INHERIT) ? TRUE : FALSE);
    HandleInfo.ProtectFromClose = ((dwFlags & HANDLE_FLAG_PROTECT_FROM_CLOSE) ? TRUE : FALSE);
    Status = NtSetInformationObject (hObject,
				     ObjectHandleInformation,
				     &HandleInfo,
				     sizeof(OBJECT_HANDLE_ATTRIBUTE_INFORMATION));
    if(!NT_SUCCESS(Status))
    {
      SetLastErrorByStatus (Status);
      return FALSE;
    }
    
    return TRUE;
  }
  else
  {
    SetLastErrorByStatus (Status);
    return FALSE;
  }
}


/*
 * @implemented
 */
BOOL STDCALL CloseHandle(HANDLE  hObject)
/*
 * FUNCTION: Closes an open object handle
 * PARAMETERS:
 *       hObject = Identifies an open object handle
 * RETURNS: If the function succeeds, the return value is nonzero
 *          If the function fails, the return value is zero
 */
{
   PRTL_USER_PROCESS_PARAMETERS Ppb;
   NTSTATUS Status;
   
   Ppb = NtCurrentPeb()->ProcessParameters;
   switch ((ULONG)hObject)
   {
     case STD_INPUT_HANDLE:
       hObject = Ppb->hStdInput;
       break;
     case STD_OUTPUT_HANDLE:
       hObject = Ppb->hStdOutput;
       break;
     case STD_ERROR_HANDLE:
       hObject = Ppb->hStdError;
       break;
   }
   
   if (IsConsoleHandle(hObject))
     {
	return(CloseConsoleHandle(hObject));
     }
   
   Status = NtClose(hObject);
   if (!NT_SUCCESS(Status))
     {     
	SetLastErrorByStatus (Status);
	return FALSE;
     }
   
   return TRUE;
}


/*
 * @implemented
 */
BOOL STDCALL DuplicateHandle(HANDLE hSourceProcessHandle,
				HANDLE hSourceHandle,
				HANDLE hTargetProcessHandle,
				LPHANDLE lpTargetHandle,
				DWORD dwDesiredAccess,
				BOOL bInheritHandle,
				DWORD dwOptions)
{
   PRTL_USER_PROCESS_PARAMETERS Ppb;
   DWORD SourceProcessId, TargetProcessId;
   NTSTATUS Status;
   
   Ppb = NtCurrentPeb()->ProcessParameters;
   switch ((ULONG)hSourceHandle)
   {
     case STD_INPUT_HANDLE:
       hSourceHandle = Ppb->hStdInput;
       break;
     case STD_OUTPUT_HANDLE:
       hSourceHandle = Ppb->hStdOutput;
       break;
     case STD_ERROR_HANDLE:
       hSourceHandle = Ppb->hStdError;
       break;
   }
   
   if (IsConsoleHandle(hSourceHandle))
   {
      if (FALSE == InternalGetProcessId(hSourceProcessHandle, &SourceProcessId) || 
	  FALSE == InternalGetProcessId(hTargetProcessHandle, &TargetProcessId) ||
	  SourceProcessId != TargetProcessId ||
	  SourceProcessId != GetCurrentProcessId())
      {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      *lpTargetHandle = DuplicateConsoleHandle(hSourceHandle, dwDesiredAccess, bInheritHandle, dwOptions);
      return *lpTargetHandle != INVALID_HANDLE_VALUE ? TRUE : FALSE;
   }
      
   Status = NtDuplicateObject(hSourceProcessHandle,
			      hSourceHandle,
			      hTargetProcessHandle,
			      lpTargetHandle,
			      dwDesiredAccess,
			      (BOOLEAN)bInheritHandle,
			      dwOptions);
   if (!NT_SUCCESS(Status))
     {
	SetLastErrorByStatus (Status);
	return FALSE;
     }
   
   return TRUE;
}


/*
 * @implemented
 */
UINT STDCALL SetHandleCount(UINT nCount)
{
   return(nCount);
}

/* EOF */
