/*
*  Copyright (C) 2014 Orbitech
*
*  Authors: xqrzd
*
*  This program is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License version 2 as
*  published by the Free Software Foundation.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
*  MA 02110-1301, USA.
*/

#include <initguid.h>
#include "Context.h"
#include "Utility.h"

NTSTATUS HzrFilterGetFileSize(
	_In_ PFLT_INSTANCE Instance,
	_In_ PFILE_OBJECT FileObject,
	_Out_ PLARGE_INTEGER Size)
{
	NTSTATUS status;
	FILE_STANDARD_INFORMATION standardInfo;

	status = FltQueryInformationFile(
		Instance,
		FileObject,
		&standardInfo,
		sizeof(FILE_STANDARD_INFORMATION),
		FileStandardInformation,
		NULL);

	if (NT_SUCCESS(status))
		*Size = standardInfo.EndOfFile;

	return status;
}

BOOLEAN HzrFilterIsPrefetchEcpPresent(
	_In_ PFLT_FILTER Filter,
	_In_ PFLT_CALLBACK_DATA Data)
{
	NTSTATUS status;
	PECP_LIST ecpList;
	PVOID ecpContext;

	status = FltGetEcpListFromCallbackData(Filter, Data, &ecpList);

	if (NT_SUCCESS(status) && (ecpList != NULL))
	{
		status = FltFindExtraCreateParameter(Filter,
			ecpList,
			&GUID_ECP_PREFETCH_OPEN,
			&ecpContext,
			NULL);

		if (NT_SUCCESS(status))
		{
			if (!FltIsEcpFromUserMode(Filter, ecpContext))
				return TRUE;
		}
	}

	return FALSE;
}

BOOLEAN HzrFilterIsPrefetchContextPresent(
	_In_ PFLT_INSTANCE Instance,
	_In_ PFILE_OBJECT FileObject)
{
	NTSTATUS status;
	PFILTER_STREAMHANDLE_CONTEXT context;
	BOOLEAN prefetchOpen = FALSE;

	status = FltGetStreamHandleContext(
		Instance,
		FileObject,
		&context);

	if (NT_SUCCESS(status))
	{
		prefetchOpen = context->PrefetchOpen;
		FltReleaseContext(context);
	}

	return prefetchOpen;
}