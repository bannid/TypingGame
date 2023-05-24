#include "typinggame_debug.h"
#include "win32_fileapi.h"

bool Win32ReadEntireFile(const char *FileName, win32_file *FileStruct){
    FileStruct->Handle = CreateFileA(
                                     FileName,
                                     GENERIC_READ,
                                     FILE_SHARE_READ,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_ATTRIBUTE_NORMAL,
                                     NULL
                                     );
	if (FileStruct->Handle == INVALID_HANDLE_VALUE) {
		return false;
	}
	
    FileStruct->Size = GetFileSize(
                                   FileStruct->Handle,
                                   NULL
                                   );
	if (FileStruct->Size == INVALID_FILE_SIZE) {
		CloseHandle(FileStruct->Handle);
		return false;
	}
    u8 *Buffer = (u8*)VirtualAlloc(NULL,FileStruct->Size,MEM_COMMIT,PAGE_READWRITE);
	DWORD BytesRead = 0;
	DWORD TotalNumberOfBytesRead  = 0;
	while(TotalNumberOfBytesRead < FileStruct->Size){
		BOOL Result = ReadFile(
                               FileStruct->Handle,
                               (LPVOID)Buffer,
                               FileStruct->Size,
                               &BytesRead,
                               NULL
                               );
		if(Result == FALSE){
			CloseHandle(FileStruct->Handle);
			VirtualFree(Buffer,FileStruct->Size,MEM_RELEASE);
			return false;
		}
		
        TotalNumberOfBytesRead = BytesRead;
	}
	ASSERT(TotalNumberOfBytesRead == FileStruct->Size);
	FileStruct->Data = Buffer;
	return true;
}