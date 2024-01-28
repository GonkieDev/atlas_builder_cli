#if !OS_WINDOWS
# error "os_win32.c being included but OS_WINDOWS is 0"
#endif

#pragma push_macro("function")
#undef function

# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN 1
# endif
# include <windows.h>
# include <timeapi.h> // timeBeginPeriod
# include <shellapi.h> // DragQueryFileW & DragQueryPoint

//- gsp: Post windows headers cleanup 
//#undef WIN32_LEAN_AND_MEAN
#pragma pop_macro("function")

//////////////////////
//~ gsp: Memory functions
function u64
OS_PageSize(void)
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return info.dwPageSize;
}

function void *
OS_Reserve(u64 size)
{
    u64 gbSnappedSize = size;
    gbSnappedSize += GB(1) - 1;
    gbSnappedSize -= gbSnappedSize % GB(1);
    void *ptr = VirtualAlloc(0, gbSnappedSize, MEM_RESERVE, PAGE_NOACCESS);
    return ptr;
}

function void
OS_Release(void *ptr, u64 size)
{
    (void)size;
    VirtualFree(ptr, 0, MEM_RELEASE);
}

function void
OS_Commit(void *ptr, u64 size)
{
    u64 page_snapped_size = size;
    page_snapped_size += OS_PageSize() - 1;
    page_snapped_size -= page_snapped_size%OS_PageSize();
    VirtualAlloc(ptr, page_snapped_size, MEM_COMMIT, PAGE_READWRITE);
}

function void
OS_Decommit(void *ptr, u64 size)
{
    VirtualFree(ptr, size, MEM_DECOMMIT);
}

//////////////////////
//~ gsp: File functions
function void
OS_W32_FillFileInfo(M_Arena *arena, WIN32_FIND_DATAA *ffd, OS_FileInfo *outFileInfo)
{
    MemoryZeroStruct(outFileInfo);
    
    outFileInfo->filenameLen = strlen(ffd->cFileName);
    outFileInfo->filename = M_PushArray(arena, u8, outFileInfo->filenameLen+1);
    MemoryCopy(outFileInfo->filename, ffd->cFileName, outFileInfo->filenameLen);
    if (FILE_ATTRIBUTE_DIRECTORY & ffd->dwFileAttributes)
        outFileInfo->isDir = 1;
}

//- File iters
function b32
OS_FileIterStart(M_Arena *arena, char *searchDir, OS_Handle *outIterHandle, OS_FileInfo *outFileInfo)
{
    ScratchArenaScope(scratch, &arena, 1)
    {
        char *searchDirFormatted = 0;
        {
            u32 searchDirLen = strlen(searchDir);
            searchDirFormatted = M_PushArray(scratch.arena, u8, searchDirLen + 2);
            MemoryCopy(searchDirFormatted, searchDir, searchDirLen);
            searchDirFormatted[searchDirLen] = '*';
        }
        
        WIN32_FIND_DATAA ffd = {0};
        HANDLE handle = FindFirstFileA(searchDirFormatted, &ffd);
        if (INVALID_HANDLE_VALUE == handle)
        {
            return 0;
        }
        
        OS_Handle osHandle = { .a = (u64)handle };
        MemoryCopyStruct(outIterHandle, &osHandle);
        OS_W32_FillFileInfo(arena, &ffd, outFileInfo);
    }
    
    return 1;
}

// NOTE(gsp): returns non-zero if file was found
function b32
OS_FileIterGetNextFile(M_Arena *arena, OS_Handle iterHandle, OS_FileInfo *outFileInfo)
{
    HANDLE handle = (HANDLE)iterHandle.a;
    if (INVALID_HANDLE_VALUE == handle)
        return 0;
    
    WIN32_FIND_DATAA ffd = {0};
    if (!FindNextFileA(handle, &ffd))
        return 0;
    OS_W32_FillFileInfo(arena, &ffd, outFileInfo);
    
    return 1;
}

function void
OS_FileIterEnd(OS_Handle iterHandle)
{
    
}
