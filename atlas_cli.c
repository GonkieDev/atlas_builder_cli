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
#include "stb_image_write.h"

// NOTE(gsp): [c] includes are at the end of the file

///////////////////////////////////
//~ Types
read_only Vec2S32 defaultAtlasSize = { 1000, 1000 };
read_only char *defaultImgExtensions[] = { ".png", ".jpg", ".jpeg", ".bmp" };
read_only u32 defaultImgExtensionsCount = ArrayCount(defaultImgExtensions);

typedef struct FileInfoNode FileInfoNode;
struct FileInfoNode
{
    char *filepath; // includes filename
    char *filename;
    FileInfoNode *next;
};

typedef struct
{
    FileInfoNode *first;
    FileInfoNode *last;
} FileInfoList;

typedef struct
{
    u8 r,g,b,a;
} PixelRGBA;

typedef struct
{
    M_Atlas *allocator;
    PixelRGBA *pixels;
    Vec2S32 size;
} ImageAtlas;

///////////////////////////////////
//~
char *
StringAppend(M_Arena *arena, char *a, char *b)
{
    u64 aLen = strlen(a);
    u64 bLen = strlen(b);
    u64 lenSum = aLen + bLen;
    char *result = M_PushArray(arena, char, lenSum + 1);
    MemoryCopy(result,               a,         aLen);
    MemoryCopy(result + aLen,        b,         bLen);
    return result;
}

char *
StringCopy(M_Arena *arena, char *str)
{
    u64 strLen = strlen(str);
    char *result = M_PushArray(arena, char, strLen+1);
    MemoryCopy(result, str, strLen);
    return result;
}

b32
StringEndsIn(char *str, char *endsIn)
{
    u64 endsInLen = strlen(endsIn);
    u64 strLen = strlen(str);
    if (strLen < endsInLen)
        return 0;
    
    u64 startIdx = strLen - endsInLen;
    for (u64 i = 0; i < endsInLen; ++i)
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
        searchDir = StringAppend(scratch.arena, searchDir, "/");
        char *searchDirFormatted = StringAppend(scratch.arena, searchDir, "*");
        
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
                    node->filename = StringCopy(arena, fileInfo.filename);
                    QueuePush(fileInfoList->first, fileInfoList->last, node);
                }
            }
            while (OS_FileIterGetNextFile(scratch.arena, iterHandle, &fileInfo));
        }
        else
        {
            printf("%s does not exist.\n", searchDir);
        }
        // TODO(gsp): close handle
    }
}

ImageAtlas
ImageAtlasMake(M_Arena *arena, Vec2S32 size)
{
    ImageAtlas imageAtlas = {0};
    imageAtlas.allocator = M_AtlasInit(arena, size);
    imageAtlas.size = size;
    imageAtlas.pixels = M_PushArray(arena, PixelRGBA, size.x * size.y);
    return imageAtlas;
}

void
PrintUsage(void)
{
    printf("Usage: atlas_cli.exe 'input/path' [-d x y | -o ./my/output/path]\n");
}

void
PrintHelp(void)
{
    printf("Help:\n");
    PrintUsage();
    printf("\t'-h' to print help.\n");
    printf("\t'-d' to specify dimensions. e.g. '-d 2000 2000' creates a 2000 by 2000 atlas. Default is %d by %d.\n", defaultAtlasSize.x, defaultAtlasSize.y);
    printf("\t'-o' to specify output path. e.g. '-o ./my_atlas.png'. Default is 'atlas.png'\n");
    printf("atlas_cli will look for any files that end in: ");
    for (u32 i = 0; i < defaultImgExtensionsCount; ++i)
    {
        printf("%s ", defaultImgExtensions[i]);
    }
    printf("\n");
}

int
main(int argc, char **argv)
{
    //- Init
    InitMainThreadContext();
    M_ArenaTemp scratch = GetScratch(0, 0);
    
    //- Default settings
    char *searchDir = 0;
    char *outFilepath = "./atlas.png";
    
    Vec2S32 atlasSize = defaultAtlasSize;
    char **imgExtensions = defaultImgExtensions;
    u32 imgExtensionsCount = defaultImgExtensionsCount;
    
    //- CLI inputs
    enum
    {
        CLI_InputPath,
        CLI_OutputPath,
        CLI_Dims,
        CLI_Help,
        
        CLI_COUNT
    };
    
    char *cliFlags[CLI_COUNT] = {
        [CLI_InputPath]  = "",
        [CLI_OutputPath] = "-o",
        [CLI_Dims] = "-d",
        [CLI_Help] = "-h",
    };
    
    b32 printHelp = 0;
    
    u32 flagType = CLI_InputPath;
    for (i32 i = 1; i < argc; ++i)
    {
        char *arg = argv[i];
        
        b32 argIsFlag = 0;
        for (u32 argTypeIdx = 1; argTypeIdx < CLI_COUNT; ++argTypeIdx)
        {
            char *cliFlag = cliFlags[argTypeIdx];
            if (strcmp(cliFlag, arg) == 0)
            {
                flagType = argTypeIdx;
                argIsFlag = 1;
                break;
            }
        }
        
        if (flagType == CLI_Help)
        {
            printHelp = 1;
            break;
        }
        
        if (argIsFlag) continue;
        
        switch (flagType)
        {
            case CLI_InputPath:
            {
                searchDir = arg;
            }break;
            
            case CLI_OutputPath:
            {
                outFilepath = arg;
            }break;
            
            case CLI_Dims:
            {
                // TODO(gsp): dims
            }break;
        }
        
        flagType = CLI_InputPath;
    }
    
    if (printHelp)
    {
        PrintHelp();
        return 0;
    }
    
    if (!searchDir)
    {
        printf("Directory to search not provided.\n");
        PrintUsage();
        return 0;
    }
    
    //- Initial prints
    printf("Sprite CLI !\n");
    
    printf("Settings:\n");
    printf("\tInput directory: %s\n", searchDir);
    printf("\tAtlas size: %dx%d\n", atlasSize.x, atlasSize.y);
    printf("\tOutput file path: %s\n", outFilepath);
    printf("\tFile extensions allowed: ");
    for (u32 i = 0; i < imgExtensionsCount; ++i)
    {
        printf("%s ", imgExtensions[i]);
    }
    printf("\n");
    printf("Starting...\n");
    
    //-
    ImageAtlas imageAtlas = ImageAtlasMake(scratch.arena, atlasSize);
    
    // NOTE(gsp): get all files in subdir
    FileInfoList infoList = {0};
    GetAllFilesFromDirAndSubdirs(scratch.arena, searchDir, &infoList);
    u32 imagesCount = 0;
    
    for (FileInfoNode *node = infoList.first; node != 0; node = node->next)
    {
        b32 fileExtensionMatches = 0;
        for (u32 i = 0; i < imgExtensionsCount; ++i)
        {
            if (StringEndsIn(node->filepath, imgExtensions[i]))
            {
                fileExtensionMatches = 1;
                break;
            }
        }
        
        if (!fileExtensionMatches)
            continue;
        
        i32 width, height, channels;
        PixelRGBA *pixels = (PixelRGBA *)stbi_load(node->filepath, &width, &height, &channels, 4);
        if (!pixels)
        {
            printf("Error: Couldn't read %s\n", node->filepath);
        }
        else
        {
            printf("\t%s\t(w: %d\th: %d)\n", node->filename, width, height);
            
            Rng2S32 region = M_AtlasRegionAlloc(scratch.arena, imageAtlas.allocator, V2S32(width, height));
            Vec2S32 regionDims = Dim2S32(region);
            if (regionDims.x == 0 || regionDims.y == 0)
            {
                // TODO(gsp): add more atlases
                printf("Error: Atlas full\n");
                Assert(0);
                break;
            }
            
            // Write to memory
            for (i32 y = 0; y < height; ++y)
            {
                PixelRGBA *dst = imageAtlas.pixels + ((region.min.y + y)*imageAtlas.size.x) + region.min.x;
                PixelRGBA *src = pixels + width*y;
                MemoryCopy(dst, src, regionDims.x * sizeof(PixelRGBA));
            }
            
            imagesCount++;
        }
        stbi_image_free(pixels);
    }
    
    if (!imagesCount)
    {
        printf("Could not find any images.\n");
        return 0;
    }
    
    //- output to file
    if (stbi_write_png(outFilepath, imageAtlas.size.x, imageAtlas.size.y, 4, (void*)imageAtlas.pixels, 0))
    {
        printf("Successfully wrote to %s\n", outFilepath);
    }
    else
    {
        printf("Failed to write to %s\n", outFilepath);
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


#if COMPILER_MSVC
# pragma warning( push, 0 )
#endif

#define  STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#if OS_WINDOWS
# define STBIW_WINDOWS_UTF8
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#if COMPILER_MSVC
# pragma warning( pop )
#endif
