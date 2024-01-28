///////////////////////////////////
//~ gsp: [h] includes
#include <stdio.h>
#include <string.h>

#include "base_context_detection.h"
#include "base_types.h"
#include "base_math.h"
#include "base_memory.h"
#include "base_thread_context.h"
#include "os.h"

#include "stb_image.h"

// NOTE(gsp): [c] includes are at the end of the file

///////////////////////////////////
//~ Types
typedef struct FileInfoNode FileInfoNode;
struct FileInfoNode
{
    char *filepath;
    FileInfoNode *next;
};

typedef struct
{
    FileInfoNode *first;
    FileInfoNode *last;
} FileInfoList;

///////////////////////////////////
//~
char *
StringAppend(M_Arena *arena, char *a, char *b)
{
    u32 aLen = strlen(a);
    u32 bLen = strlen(b);
    u32 lenSum = aLen + bLen;
    char *result = M_PushArray(arena, u8, lenSum + 1);
    MemoryCopy(result,               a,         aLen);
    MemoryCopy(result + aLen,        b,         bLen);
    return result;
}

b32
StringEndsIn(char *str, char *endsIn)
{
    u32 endsInLen = strlen(endsIn);
    u32 strLen = strlen(str);
    if (strLen < endsInLen)
        return 0;
    
    u32 startIdx = strLen - endsInLen;
    for (u32 i = 0; i < endsInLen; ++i)
    {
        if (str[startIdx + i] != endsIn[i])
        {
            return 0;
        }
    }
    return 1;
}

void
GetAllFilesFromDirAndSubdirs(M_Arena *arena, char *searchDir, FileInfoList *fileInfoList)
{
    ScratchArenaScope(scratch, &arena, 1)
    {
        char *searchDirFormatted = StringAppend(scratch.arena, searchDir, "/*");
        
        OS_FileInfo fileInfo;
        OS_Handle iterHandle;
        if (OS_FileIterStart(scratch.arena, searchDirFormatted, &iterHandle, &fileInfo))
        {
            do 
            {
                if ((strcmp("..", fileInfo.filename) == 0) || (strcmp(".", fileInfo.filename) == 0))
                    continue;
                
                if (fileInfo.isDir)
                {
                    char *childDir = StringAppend(scratch.arena, searchDir, fileInfo.filename);
                    GetAllFilesFromDirAndSubdirs(arena, childDir, fileInfoList);
                }
                else
                {
                    FileInfoNode *node = M_PushStruct(arena, FileInfoNode);
                    node->filepath = StringAppend(arena, searchDir, fileInfo.filename);
                    QueuePush(fileInfoList->first, fileInfoList->last, node);
                }
            }
            while (OS_FileIterGetNextFile(scratch.arena, iterHandle, &fileInfo));
        }
    }
}

int
main()
{
    InitMainThreadContext();
    M_ArenaTemp scratch = GetScratch(0, 0);
    
    printf("Sprite CLI !\n");
    
    char *searchDir = "../../test_sprites/";
    Vec2S32 atlasSize = V2S32(2000, 2000);
    
    printf("Dir: %s\n", searchDir);
    printf("Atlas size: %dx%d\n", atlasSize.x, atlasSize.y);
    
    M_Atlas *atlas = M_AtlasInit(scratch.arena, atlasSize);
    
    FileInfoList infoList = {0};
    GetAllFilesFromDirAndSubdirs(scratch.arena, searchDir, &infoList);
    
    for (FileInfoNode *node = infoList.first; node != 0; node = node->next)
    {
        printf("%s (ends in .png: %d)\n", node->filepath, StringEndsIn(node->filepath, ".png"));
    }
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