#ifndef BASE_MEMORY_INCLUDE_H
#define BASE_MEMORY_INCLUDE_H

////////////////////////////
//~ gsp: Memory Arena types

typedef struct M_Arena M_Arena;
struct M_Arena
{
    u64 size;
    u64 pos;
    u64 commitPos;
    u64 alignment;
    u64 checkpoints[4];
    u64 checkpointsCount;
};

typedef struct M_ArenaTemp M_ArenaTemp;
struct M_ArenaTemp
{
    M_Arena *arena;
    u64 pos;
};

////////////////////////////
//~ gsp: Atlas types
typedef u32 M_AtlasRegionNodeFlags;
enum
{
    M_AtlasRegionNodeFlag_Taken = (1 << 0),
};

typedef struct M_AtlasRegionNode M_AtlasRegionNode;
struct M_AtlasRegionNode
{
    M_AtlasRegionNode *parent;
    M_AtlasRegionNode *children[Corner_COUNT];
    Vec2S32 maxFreeSize[Corner_COUNT];
    u64 numAllocatedDescendants;
    M_AtlasRegionNodeFlags flags;
    u32 __pad;
};

typedef struct M_Atlas M_Atlas;
struct M_Atlas
{
    Vec2S32 rootSize;
    M_AtlasRegionNode *root;
};

////////////////////////////
//~ gsp: Constants

#if !defined(M_ARENA_COMMIT_GRANULARITY)
# define M_ARENA_COMMIT_GRANULARITY KB(4)
#endif

StaticAssert(M_ARENA_COMMIT_GRANULARITY >= sizeof(M_Arena), "M_ARENA_COMMIT_GRANULARITY must be larger than sizeof(M_Arena)");

#if !defined(M_ARENA_DECOMMIT_THRESHOLD)
# define M_ARENA_DECOMMIT_THRESHOLD MB(64)
#endif

#if !defined(M_ARENA_SIZE_ROUNDUP_GRANULARITY)
# define M_ARENA_SIZE_ROUNDUP_GRANULARITY MB(64)
#endif

#if !defined(M_ARENA_DEFAULT_ALLOCATION_SIZE)
# define M_ARENA_DEFAULT_ALLOCATION_SIZE GB(8)
#endif

////////////////////////////
//~ gsp: Arena functions

// NOTE(gsp): Allocates memory for M_Arena and initialises it and also allocates block of memory of size "size"
function M_Arena *M_ArenaAlloc(u64 size);
function M_Arena *M_ArenaAllocDefault(void);
function void M_ArenaRelease(M_Arena *arena);

// NOTE(gsp): Pushes struct to arena but does NOT memzero it
function void *M_ArenaPushNoZero(M_Arena *arena, u64 size);
// NOTE(gsp): Push into arena until it matches the alignment
function void *M_ArenaPushAligner(M_Arena *arena, u64 alignment);
// NOTE(gsp): Pushes struct to arena and memzeros it
function void *M_ArenaPush(M_Arena *arena, u64 size);

function void M_ArenaPopTo(M_Arena *arena, u64 pos);
function void M_ArenaPop(M_Arena *arena, u64 size);
function void M_ArenaClear(M_Arena *arena);
function void M_ArenaClearIgnoreCheckpoints(M_Arena *arena);

function void M_ArenaSetAutoAlignment(M_Arena *arena, u64 alignment);
function u64  M_ArenaPos(M_Arena *arena);

function void M_ArenaSetCheckpoint(M_Arena *arena);
function void M_ArenaSetCheckpointAtPos(M_Arena *arena, u64 pos);

#define M_PushStruct(arena, type) (type *)M_ArenaPush((arena), sizeof(type))
#define M_PushStructNoZero(arena, type) (type *)M_ArenaPushNoZero((arena), sizeof(type))

#define M_PushArrayNoZero(arena, type, count) (type *)M_ArenaPushNoZero((arena), sizeof(type)*(count))
#define M_PushArray(arena, type, count)       (type *)M_ArenaPush((arena), sizeof(type)*(count))
#define M_PushString(arena, type, count)              M_PushArrayNoZero(arena, type, (count) + 1)

////////////////////////////
//~ gsp: Temp arena functions
function M_ArenaTemp GetScratch(M_Arena **conflicts, u32 conflictsCount);
#define EndScratch(scratch) ArenaTempEnd(scratch)

// NOTE(gsp): do not use this function directly, use GetScratch instead
function M_ArenaTemp ArenaTempBegin(M_Arena *arena);
function void ArenaTempEnd(M_ArenaTemp temp);

#define ScratchArenaScope(name, conflicts, conflictsCount) M_ArenaTemp (name); DeferLoop(((name) = GetScratch((conflicts), (conflictsCount))), EndScratch((name)))


////////////////////////////
//~ gsp: Atlas functions
function M_Atlas *M_AtlasInit(M_Arena *arena, Vec2S32 size);
function Rng2S32 M_AtlasRegionAlloc(M_Arena *arena, M_Atlas *atlas, Vec2S32 size);
function void M_AtlasRegionRelease(M_Atlas *atlas, Rng2S32 region);



////////////////////////////
//~ End of file
#endif //BASE_MEMORY_INCLUDE_H
