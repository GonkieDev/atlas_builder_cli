#ifndef BASE_THREAD_CONTEXT_INCLUDE
#define BASE_THREAD_CONTEXT_INCLUDE

typedef struct ThreadContext ThreadContext;
struct ThreadContext
{
    M_Arena *arenas[2];
    b64 isMainThread;
};

function void InitMainThreadContext(void);
function void InitThreadContext(void);
function b32  IsMainThread(void);


//////////////////
//~ End of file
#endif //BASE_THREAD_CONTEXT_INCLUDE
