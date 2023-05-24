#include <strsafe.h>
#include <tchar.h>
#include <Windows.h>

#define READ_JOURNAL_BUFFER_SIZE (1024 * 1024)
#define ISO_DATETIME_LEN 26
#define ROW_LEN 4096
#define ID_LEN 33

#define Add2Ptr(Ptr,Inc) ((PVOID)((PUCHAR)(Ptr) + (Inc)))

HANDLE hUsnFileHandle = INVALID_HANDLE_VALUE;
HANDLE hCsvFileHandle = INVALID_HANDLE_VALUE;
BOOL bStatus;
HRESULT hResult;
USN_JOURNAL_DATA_V1 UsnJournalData;
READ_USN_JOURNAL_DATA_V1 ReadUsnJournalDataV1;
PUSN_RECORD_V3 UsnRecordV3;
DWORD dwBytesReturned;
DWORD dwWritten;
PVOID lpBuffer = NULL;
TCHAR strIsoDateTime[ISO_DATETIME_LEN];
TCHAR strFileId[ID_LEN];
TCHAR strParentFileId[ID_LEN];
TCHAR strBuf[ROW_LEN];
LONG lCounter = 0;
PTCHAR pszVolumeName;
PTCHAR pszCsvFileName;



__inline PUSN_RECORD_V3 NextUsnRecord(PUSN_RECORD_V3 currentRecord)
{
	ULONGLONG newRecord;
	(PUSN_RECORD_V3)newRecord = currentRecord;
	newRecord += currentRecord->RecordLength;
	if (newRecord & 8 - 1) // align if needed
	{
		newRecord &= -8;
		newRecord += 8;
	}
	return((PUSN_RECORD_V3)newRecord);
}

PTCHAR TimeStampToIso8601(
	LARGE_INTEGER* timeStamp,
	PTCHAR pszBuffer,
	size_t cchBufferSize
)
{
	SYSTEMTIME systemTime;
	if (pszBuffer == NULL)
	{
		return NULL;
	}
	if (FileTimeToSystemTime((PFILETIME)timeStamp, &systemTime))
	{
		StringCchPrintf(pszBuffer,
			ISO_DATETIME_LEN,
			TEXT("%04i-%02i-%02iT%02i:%02i:%02i.%03iZ"),
			systemTime.wYear,
			systemTime.wMonth,
			systemTime.wDay,
			systemTime.wHour,
			systemTime.wMinute,
			systemTime.wSecond,
			systemTime.wMilliseconds
		);
	}
	return pszBuffer;
}

PTCHAR FileId128ToHex(
	PUCHAR fileId128,
	PTCHAR pszBuffer,
	size_t cchBufferSize
)
{
	ULONGLONG lowerPart;
	ULONGLONG higherPart;
	if (pszBuffer == NULL)
	{
		return NULL;
	}
	lowerPart = *(PULONGLONG)fileId128;
	fileId128 = Add2Ptr(fileId128, sizeof(ULONGLONG));
	higherPart = *(PULONGLONG)fileId128;
	StringCchPrintf(
		pszBuffer,
		cchBufferSize,
		TEXT("%016I64x%016I64x"),
		higherPart,
		lowerPart
	);
	return pszBuffer;
}

int _tmain(int argc, PTCHAR argv[])
{
	if (argc == 3)
	{
		pszVolumeName = argv[1];
		pszCsvFileName = argv[2];
	}
	else
	{
		_tprintf(TEXT("Usage: NTFSJournal2CSV.exe \\\\.\\C: c:\\journal.csv\r\n"));
		return ERROR_INVALID_PARAMETER;
	}

	_tprintf(TEXT("Trying to open %s ..."), pszVolumeName);
	hUsnFileHandle = CreateFile(
		pszVolumeName,
		FILE_WRITE_ATTRIBUTES,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hUsnFileHandle)
	{
		_tprintf(TEXT("\r\nERROR: CreateFile() returned %u\r\n"), GetLastError());
		return GetLastError();
	}
	else
	{
		_tprintf(TEXT(" Success.\r\n"));
	}
	_tprintf(TEXT("Trying to open %s ..."), pszCsvFileName);
	hCsvFileHandle = CreateFile(
		pszCsvFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (INVALID_HANDLE_VALUE == hCsvFileHandle)
	{
		_tprintf(TEXT("\r\nERROR: CreateFile() returned %u\r\n"), GetLastError());
		return GetLastError();
	}
	else
	{
		_tprintf(TEXT(" Success.\r\n"));
	}

	_tprintf(TEXT("Sending FSCTL_QUERY_USN_JOURNAL\r\n"));
	bStatus = DeviceIoControl(
		hUsnFileHandle,
		FSCTL_QUERY_USN_JOURNAL,
		NULL,
		0,
		&UsnJournalData,
		sizeof(UsnJournalData),
		&dwBytesReturned,
		NULL
	);

	if (!bStatus)
	{
		_tprintf(
			TEXT("ERROR: DeviceIoControl(FSCTL_QUERY_USN_JOURNAL...) returned %u\r\n"),
			GetLastError()
		);
		return GetLastError();
	}

	_tprintf(TEXT("Journal found on %s\r\n"), pszVolumeName);
	_tprintf(TEXT(" UsnJournalID    : 0x%016llx\r\n"), UsnJournalData.UsnJournalID);
	_tprintf(TEXT(" FirstUsn        : 0x%016llx\r\n"), UsnJournalData.FirstUsn);
	_tprintf(TEXT(" NextUsn         : 0x%016llx\r\n"), UsnJournalData.NextUsn);
	_tprintf(TEXT(" LowestValidUsn  : 0x%016llx\r\n"), UsnJournalData.LowestValidUsn);
	_tprintf(TEXT(" MaxUsn          : 0x%016llx\r\n"), UsnJournalData.MaxUsn);
	_tprintf(TEXT(" MaximumSize     : 0x%016llx\r\n"), UsnJournalData.MaximumSize);
	_tprintf(TEXT(" AllocationDelta : 0x%016llx\r\n"), UsnJournalData.AllocationDelta);
	_tprintf(TEXT(" MinSupportedMajorVersion : %u\r\n"), UsnJournalData.MinSupportedMajorVersion);
	_tprintf(TEXT(" MaxSupportedMajorVersion : %u\r\n"), UsnJournalData.MaxSupportedMajorVersion);

	ReadUsnJournalDataV1.UsnJournalID = UsnJournalData.UsnJournalID;
	ReadUsnJournalDataV1.ReasonMask = ~0;
	ReadUsnJournalDataV1.ReturnOnlyOnClose = FALSE;
	ReadUsnJournalDataV1.Timeout = 0;
	ReadUsnJournalDataV1.MinMajorVersion = 2;
	ReadUsnJournalDataV1.MaxMajorVersion = 4;

	lpBuffer = (PVOID)malloc(READ_JOURNAL_BUFFER_SIZE);

	if (!lpBuffer)
	{
		_tprintf(TEXT("ERROR: Cannot allocate buffer.\r\n"));
		return ERROR_OUTOFMEMORY;
	}

	_tprintf(TEXT("Processing Journal...\r\n"));

	while (TRUE)
	{
		memset(lpBuffer, 0, READ_JOURNAL_BUFFER_SIZE);
		bStatus = DeviceIoControl(
			hUsnFileHandle,
			FSCTL_READ_USN_JOURNAL,
			&ReadUsnJournalDataV1,
			sizeof(ReadUsnJournalDataV1),
			lpBuffer,
			READ_JOURNAL_BUFFER_SIZE,
			&dwBytesReturned,
			NULL
		);

		if (!bStatus)
		{
			if ((ERROR_HANDLE_EOF == GetLastError()) || (ERROR_WRITE_PROTECT == GetLastError())) //this is fine
			{
				break;
			}
			else
			{
				_tprintf(TEXT("ERROR: DeviceIoControl(FSCTL_READ_USN_JOURNAL...) returned %u\r\n"), GetLastError());
				return GetLastError();
			}
		}

		if (dwBytesReturned < sizeof(ULONGLONG) + sizeof(USN_RECORD))
		{
			break;
		}

		UsnRecordV3 = (PUSN_RECORD_V3)((PBYTE)lpBuffer + sizeof(ULONGLONG));
		while (((PBYTE)UsnRecordV3 < (PBYTE)lpBuffer + dwBytesReturned) && ((PBYTE)UsnRecordV3 + UsnRecordV3->RecordLength <= (PBYTE)lpBuffer + dwBytesReturned))
		{
			lCounter++;
			switch (UsnRecordV3->MajorVersion) //ver 2, 3, and 4 have the same structure fields at the beginning. Even if it is not ver 3, the code is ok.
			{
				case 3: //only ver 3 is supported, as it is the only one happening in disks I had a chance to analyze.
					hResult= StringCchPrintf(
						strBuf,
						ROW_LEN,
						TEXT("%lld\t0x%08x\t0x%08x\t0x%08x\t0x%08x\t%s\t%s\t%s\t%.*s\r\n"),
						UsnRecordV3->Usn,
						UsnRecordV3->Reason,
						UsnRecordV3->SourceInfo,
						UsnRecordV3->SecurityId,
						UsnRecordV3->FileAttributes,
						TimeStampToIso8601(&UsnRecordV3->TimeStamp, strIsoDateTime, ISO_DATETIME_LEN),
						FileId128ToHex(UsnRecordV3->FileReferenceNumber.Identifier, strFileId, _countof(strFileId)),
						FileId128ToHex(UsnRecordV3->ParentFileReferenceNumber.Identifier, strParentFileId, _countof(strParentFileId)),
						UsnRecordV3->FileNameLength / sizeof(WCHAR), //wchar as it is about wide string defined in struct; value used to cut name in the next param
						(PWSTR)Add2Ptr(UsnRecordV3, UsnRecordV3->FileNameOffset) //always PWSTR as it is how it goes in struct
					);
				if (S_OK != hResult)
				{
					_tprintf(TEXT("ERROR: StringCchPrintf returned %u\r\n"), hResult);
					return ERROR_INSUFFICIENT_BUFFER;
				}
#pragma warning( disable : 4267 ) //size_t to dword. Should be safe because size of TCHAR should not overflow DWORD
					bStatus = WriteFile(
						hCsvFileHandle,
						strBuf,
						_tcslen(strBuf) * sizeof(TCHAR),
						&dwWritten,
						NULL
					);
#pragma warning( default : 4267 ) //back to normal
					if (!bStatus)
					{
						_tprintf(
							TEXT("ERROR: WriteFile() returned %u\r\n"),
							GetLastError()
						);
						return GetLastError();
					}
					break; //ver 3

				default:
					_tprintf(TEXT("Unknown record version. Ver. %i.%i. Length: %i\r\n"),
						UsnRecordV3->MajorVersion,
						UsnRecordV3->MinorVersion,
						UsnRecordV3->RecordLength);
					return ERROR_UNSUPPORTED_TYPE;
					break;
			}
			UsnRecordV3 = NextUsnRecord(UsnRecordV3);
			if (0 == (lCounter % 100000))
			{
				_tprintf(TEXT("."));
			}
		} //inner while
		ReadUsnJournalDataV1.StartUsn = *(USN*)lpBuffer;
	} //while true
	_tprintf(TEXT("\r\nDone. %ld entries processed.\r\n"), lCounter);
	CloseHandle(hUsnFileHandle);
	CloseHandle(hCsvFileHandle);
	return 0;
}

