#include "windows.h"
#include "defines.h"
#include <stdlib.h>
#include <malloc.h>

// File I/O
enum FileFlags
{
    INVALID_HANDLE = 0x0,
    ACCESS_READ = 0x1,
    ACCESS_WRITE = 0x2,
    TYPE_TEXT = 0x10,
    TYPE_BINARY = 0x20,
};

struct FileHandle
{
    void *Data;
    u32 Size;
    b32 Flags;
};

inline void FileFree(FileHandle &File)
{
    free(File.Data);
    File.Size = 0;
    File.Flags = INVALID_HANDLE;
}

inline FileHandle FileAlloc(u32 Size, b32 Flags)
{
    FileHandle Handle = {};

    Handle.Data = malloc(Size);
    Handle.Size = Size;
    Handle.Flags = Flags;

    return Handle;
}

FileHandle GetAllDataFromFile(const char *FilePath, b32 FileFlags = ACCESS_READ)
{
    FileHandle Handle = {};

    u32 DesiredAccess = CheckFlags(FileFlags, ACCESS_READ) ? GENERIC_READ : 0;
    DesiredAccess |= CheckFlags(FileFlags, ACCESS_WRITE) ? GENERIC_WRITE : 0;
    b32 IsText = FileFlags & TYPE_TEXT;

    HANDLE File = CreateFile(FilePath, DesiredAccess, 0, 0, OPEN_EXISTING,
                             FILE_FLAG_SEQUENTIAL_SCAN, 0);
    if (File != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if (GetFileSizeEx(File, &FileSize))
        {
            // NOTE(valentino): +1 for the null terminated character we will add later;
            u32 Size = IsText ? FileSize.LowPart + 1 : FileSize.LowPart;
            Handle = FileAlloc(Size, FileFlags);

            DWORD BytesRead;
            if (ReadFile(File, Handle.Data, Size, &BytesRead, 0))
            {
                if (IsText)
                {
                    ((char *)Handle.Data)[Size - 1] = '\0';
                }
            }
            else
            {
                FileFree(Handle);
                printf("Error: Reading file %s failed.", FilePath);
            }
        }

        CloseHandle(File);
    }
    else if (GetLastError() == ERROR_FILE_NOT_FOUND)
    {
        printf("Error: could not open file %s\n", FilePath);
    }

    return Handle;
}