#ifndef OS_CORE_INCLUDE_H
#define OS_CORE_INCLUDE_H

//////////////////////
//~ gsp: Memory functions
function u64   OS_PageSize(void);
function void *OS_Reserve(u64 size);
function void  OS_Release(void *ptr, u64 size);
function void  OS_Commit(void *ptr, u64 size);
function void  OS_Decommit(void *ptr, u64 size);

//////////////////////////////////////////////////////
//~ END OF FILE
#endif //OS_CORE_INCLUDE_H