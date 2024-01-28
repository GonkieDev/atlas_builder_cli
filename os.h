#ifndef OS_CORE_INCLUDE_H
#define OS_CORE_INCLUDE_H

//////////////////////
//~ gsp: OS Types

typedef struct
{
    u64 a;
} OS_Handle;

typedef struct
{
    char *filename; // allocated in arena
    u32   filenameLen;
    b32   isDir;
} OS_FileInfo;

//////////////////////
//~ gsp: Memory functions
function u64   OS_PageSize(void);
function void *OS_Reserve(u64 size);
function void  OS_Release(void *ptr, u64 size);
function void  OS_Commit(void *ptr, u64 size);
function void  OS_Decommit(void *ptr, u64 size);

//////////////////////
//~ gsp: File functions

//- File iters
function b32  OS_FileIterStart(M_Arena *arena, char *searchDir, OS_Handle *outIterHandle, OS_FileInfo *outFileInfo);
// NOTE(gsp): returns non-zero if file was found
function b32  OS_FileIterGetNextFile(M_Arena *arena, OS_Handle iterHandle, OS_FileInfo *outFileInfo);
function void OS_FileIterEnd(OS_Handle iterHandle);


//////////////////////////////////////////////////////
//~ END OF FILE
#endif //OS_CORE_INCLUDE_H