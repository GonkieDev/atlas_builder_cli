#ifndef BASE_THREAD_CONTEXT_INCLUDE_C
#define BASE_THREAD_CONTEXT_INCLUDE_C

per_thread ThreadContext tctx;
StaticAssert(ArrayCount(tctx.arenas) >= 2,
             "Need at least 2 arenas in a thread context to allow for scratch arenas to work!");

function void
InitThreadContext(void)
{
    tctx.arenas[0] = M_ArenaAllocDefault();
    tctx.arenas[1] = M_ArenaAllocDefault();
}

function void
InitMainThreadContext(void)
{
    InitThreadContext();
    tctx.isMainThread = 1;
}

function b32
IsMainThread(void)
{
    return (b32)tctx.isMainThread;
}

//~ End of file
#endif // #ifndef BASE_THREAD_CONTEXT_INCLUDE_C