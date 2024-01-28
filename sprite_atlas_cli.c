///////////////////////////////////
//~ gsp: [h] includes
#include <stdio.h>

#include "base_context_detection.h"
#include "base_types.h"
#include "base_math.h"
#include "base_memory.h"
#include "base_thread_context.h"
#include "os.h"

#include "stb_image.h"

// NOTE(gsp): [c] includes are at the end of the file

///////////////////////////////////
//~
int
main()
{
    InitMainThreadContext();
    
    printf("Sprite CLI !");
    
    
}

///////////////////////////////////
//~ gsp: [c] includes
#if OS_WINDOWS
# include "os_win32.c"
#else
#endif

#include "base_thread_context.c"

#define M_ArenaImpl_Reserve  OS_Reserve
#define M_ArenaImpl_Release  OS_Release
#define M_ArenaImpl_Commit   OS_Commit
#define M_ArenaImpl_Decommit OS_Decommit
#include "base_memory.c"

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"