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