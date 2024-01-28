#ifndef BASE_MEMORY_INCLUDE_C
#define BASE_MEMORY_INCLUDE_C

//////////////////////////////////////////
//~ gsp: Memory arenas

//- gsp: Prechecks 
#if !defined(M_ArenaImpl_Reserve)
# error ArenaImpl_Reserve must be defined to use base_memory.c.
#endif
#if !defined(M_ArenaImpl_Release)
# error ArenaImpl_Release must be defined to use base_memory.c.
#endif
#if !defined(M_ArenaImpl_Commit)
# error ArenaImpl_Commit must be defined to use base_memory.c.
#endif
#if !defined(M_ArenaImpl_Decommit)
# error ArenaImpl_Decommit must be defined to use base_memory.c.
#endif

#if !defined(MemoryZero)
# error "MemoryZero must be defined to use base_memory.c."
#endif

//- gsp: Arena functions 

function M_Arena *
M_ArenaAlloc(u64 size)
{
    M_Arena *result = 0;
    
    // gsp: Round size up to nearest multiple of M_ARENA_SIZE_ROUNDUP_GRANULARITY
    u64 sizeRoundupGranularity = M_ARENA_SIZE_ROUNDUP_GRANULARITY;
    size += sizeRoundupGranularity - 1;
    size -= size % sizeRoundupGranularity;
    
    void *memory = M_ArenaImpl_Reserve(size);
    
    if (memory)
    {
        const u64 initialCommitSize = M_ARENA_COMMIT_GRANULARITY;
        Assert(initialCommitSize >= sizeof(M_Arena));
        M_ArenaImpl_Commit(memory, initialCommitSize);
        
        M_Arena *arena = (M_Arena *)memory;
        arena->pos = sizeof(M_Arena);
        arena->commitPos = initialCommitSize;
        arena->alignment = 8;
        arena->size = size;
        
        result = arena;
    }
    
    return result;
}

function M_Arena *
M_ArenaAllocDefault(void)
{
    M_Arena *result = M_ArenaAlloc(M_ARENA_DEFAULT_ALLOCATION_SIZE);
    return result;
}

function void
M_ArenaRelease(M_Arena *arena)
{
    M_ArenaImpl_Release(arena, arena->size);
}

function void *
M_ArenaPushNoZero(M_Arena *arena, u64 size)
{
    void *result = 0;
    
    if (arena->pos + size <= arena->size)
    {
        u8 *memory = (u8*)arena;
        
        // let pos = 29; let alignment = 8
        // 29 = 22 + 7
        // 29 - (29 % 8)
        u64 postAlignPos = (arena->pos + (arena->alignment - 1));
        postAlignPos -= postAlignPos % arena->alignment;
        
        u64 align = postAlignPos - arena->pos;
        result = memory + arena->pos + align;
        arena->pos += size + align;
        
        // NOTE(gsp): check if we need to commit more memory
        if (arena->commitPos < arena->pos)
        {
            u64 sizeToCommit = arena->pos - arena->commitPos;
            sizeToCommit += M_ARENA_COMMIT_GRANULARITY - 1;
            sizeToCommit -= sizeToCommit % M_ARENA_COMMIT_GRANULARITY;
            M_ArenaImpl_Commit(memory + arena->commitPos, sizeToCommit);
            arena->commitPos += sizeToCommit;
        }
    }
    else
    {
        // TODO(gsp): what to do when arena is full
        InvalidPath;
    }
    
    return result;
}

function void *
M_ArenaPushAligner_(M_Arena *arena, u64 alignment)
{
    u64 pos = arena->pos;
    u64 posRoundedUp = pos + alignment-1;
    posRoundedUp -= posRoundedUp % alignment;
    u64 sizeToAlloc = posRoundedUp - pos;
    
    void *result = 0;
    if (sizeToAlloc != 0)
    {
        result = M_ArenaPushNoZero(arena, sizeToAlloc);
    }
    return result;
}

function void *
M_ArenaPush(M_Arena *arena, u64 size)
{
    void *result = M_ArenaPushNoZero(arena, size);
    MemoryZero(result, size);
    return result;
}

function void
M_ArenaPopTo(M_Arena *arena, u64 pos)
{
    u64 minPos = sizeof(M_Arena);
    u64 newPos = Max(minPos, pos);
    
    arena->pos = newPos;
    
    u64 posAlignedToCommits = arena->pos + M_ARENA_COMMIT_GRANULARITY - 1;
    posAlignedToCommits -= posAlignedToCommits % M_ARENA_COMMIT_GRANULARITY;
    
    if (posAlignedToCommits + M_ARENA_DECOMMIT_THRESHOLD <= arena->commitPos)
    {
        u8 *memory = (u8*)arena;
        u64 sizeToDecommit = arena->commitPos - posAlignedToCommits;
        M_ArenaImpl_Decommit(memory + posAlignedToCommits, sizeToDecommit);
        arena->commitPos -= sizeToDecommit;
    }
}

function void
M_ArenaPop(M_Arena *arena, u64 size)
{
    u64 minPos = sizeof(M_Arena);
    u64 sizeToPop = Min(size, arena->pos);
    u64 newPos = arena->pos - sizeToPop;
    newPos = Max(minPos, newPos);
    M_ArenaPopTo(arena, newPos);
}

function void
M_ArenaClear(M_Arena *arena)
{
    u64 checkpointPos = arena->checkpoints[arena->checkpointsCount - 1];
    M_ArenaPopTo(arena, checkpointPos);
}

function void
M_ArenaClearIgnoreCheckpoints(M_Arena *arena)
{
    M_ArenaPopTo(arena, sizeof(M_Arena));
}

function void
M_ArenaSetAutoAlignment(M_Arena *arena, u64 alignment)
{
    arena->alignment = alignment;
}

function u64
M_ArenaPos(M_Arena *arena)
{
    u64 result = arena->pos;
    return result;
}

function void
M_ArenaSetCheckpoint(M_Arena *arena)
{
    M_ArenaSetCheckpointAtPos(arena, arena->pos);
}

function void 
M_ArenaSetCheckpointAtPos(M_Arena *arena, u64 pos)
{
    arena->checkpoints[arena->checkpointsCount++] = pos;
}

////////////////////////////
//~ gsp: Temp arena functions
function M_ArenaTemp
GetScratch(M_Arena **conflicts, u32 conflictsCount)
{
    Assert(conflictsCount < ArrayCount(tctx.arenas));
    M_Arena *scratchArena = tctx.arenas[0];
    
    for (u32 conflictingIdx = 0; conflictingIdx < conflictsCount; ++conflictingIdx)
    {
        M_Arena *conflictingArena = conflicts[conflictingIdx];
        for (u32 arenaIdx = 0; arenaIdx < ArrayCount(tctx.arenas); ++arenaIdx)
        {
            M_Arena *currArena = tctx.arenas[arenaIdx];
            if (currArena != conflictingArena)
            {
                scratchArena = currArena;
                break;
            }
        }
    }
    
    return ArenaTempBegin(scratchArena);
}

function M_ArenaTemp
ArenaTempBegin(M_Arena *arena)
{
    M_ArenaTemp result  = {0};
    result.pos = arena->pos;
    result.arena = arena;
    return result;
}

function void
ArenaTempEnd(M_ArenaTemp temp)
{
    M_ArenaPopTo(temp.arena, temp.pos);
}

//////////////////////////////////////////
//~ gsp: Memory atlas
function M_Atlas *
M_AtlasInit(M_Arena *arena, Vec2S32 size)
{
    M_Atlas *atlas = M_PushStruct(arena, M_Atlas);
    atlas->rootSize = size;
    atlas->root = M_PushStruct(arena, M_AtlasRegionNode);
    atlas->root->maxFreeSize[0] =
        atlas->root->maxFreeSize[1] =
        atlas->root->maxFreeSize[2] =
        atlas->root->maxFreeSize[3] = V2S32(size.x / 2, size.y / 2);
    return atlas;
}

function Rng2S32
M_AtlasRegionAlloc(M_Arena *arena, M_Atlas *atlas, Vec2S32 size)
{
    if (!atlas)
    {
        AssertMsg(0, "Null atlas.");
        Rng2S32 nullResult = {0};
        return nullResult;
    }
    
    Vec2S32 region_p0 = {0};
    Vec2S32 region_sz = {0};
    Corner nodeCorner = Corner_Invalid;
    M_AtlasRegionNode *node = 0;
    {
        Vec2S32 supportedSize = atlas->rootSize;
        for(M_AtlasRegionNode *n = atlas->root, *next = 0; n != 0; n = next, next = 0)
        {
            if (n->flags & M_AtlasRegionNodeFlag_Taken)
            {
                break;
            }
            
            b32 canBeAllocated = (n->numAllocatedDescendants == 0);
            if (canBeAllocated)
            {
                region_sz = supportedSize;
            }
            
            Vec2S32 childSize = {supportedSize.x/2, supportedSize.y/2};
            M_AtlasRegionNode *bestChild = 0;
            if ((childSize.x >= size.x) && (childSize.y >= size.y))
            {
                for(EachCorner(corner))
                {
                    if (n->children[corner] == 0)
                    {
                        n->children[corner] = M_PushStruct(arena, M_AtlasRegionNode);
                        n->children[corner]->parent = n;
                        n->children[corner]->maxFreeSize[Corner_00] = 
                            n->children[corner]->maxFreeSize[Corner_01] = 
                            n->children[corner]->maxFreeSize[Corner_10] = 
                            n->children[corner]->maxFreeSize[Corner_11] = V2S32(childSize.x/2, childSize.y/2);
                    }
                    if ((n->maxFreeSize[corner].x >= size.x) && (n->maxFreeSize[corner].y >= size.y))
                    {
                        bestChild = n->children[corner];
                        nodeCorner = corner;
                        Vec2S32 sideVertex = SideVertexFromCorner32(corner);
                        region_p0.x += sideVertex.x*childSize.x;
                        region_p0.y += sideVertex.y*childSize.y;
                        break;
                    }
                }
            }
            
            if(canBeAllocated && bestChild == 0)
            {
                node = n;
            }
            else
            {
                next = bestChild;
                supportedSize = childSize;
            }
        }
    }
    
    // Mark up all parents
    if (node != 0 && nodeCorner != Corner_Invalid)
    {
        node->flags |= M_AtlasRegionNodeFlag_Taken;
        if (node->parent != 0)
        {
            MemoryZeroStruct(&node->parent->maxFreeSize[nodeCorner]);
        }
        for(M_AtlasRegionNode *p = node->parent; p != 0; p = p->parent)
        {
            p->numAllocatedDescendants += 1;
            M_AtlasRegionNode *parent = p->parent;
            if (parent != 0)
            {
                Corner pCorner = (p == parent->children[Corner_00] ? Corner_00 :
                                  p == parent->children[Corner_01] ? Corner_01 :
                                  p == parent->children[Corner_10] ? Corner_10 :
                                  p == parent->children[Corner_11] ? Corner_11 :
                                  Corner_Invalid);
                if (pCorner == Corner_Invalid)
                {
                    InvalidPath;
                }
                parent->maxFreeSize[pCorner].x = Max(Max(p->maxFreeSize[Corner_00].x,
                                                         p->maxFreeSize[Corner_01].x),
                                                     Max(p->maxFreeSize[Corner_10].x,
                                                         p->maxFreeSize[Corner_11].x));
                parent->maxFreeSize[pCorner].y = Max(Max(p->maxFreeSize[Corner_00].y,
                                                         p->maxFreeSize[Corner_01].y),
                                                     Max(p->maxFreeSize[Corner_10].y,
                                                         p->maxFreeSize[Corner_11].y));
            }
        }
    }
    
    Rng2S32 result;
    result.p0 = region_p0;
    result.p1 = Add2S32(region_p0, region_sz);
    return result;
}

function void
M_AtlasRegionRelease(M_Atlas *atlas, Rng2S32 region)
{
    Vec2S32 regionSize = Dim2S32(region);
    
    Vec2S32 calcRegionSize = {0};
    M_AtlasRegionNode *node = 0;
    Corner nodeCorner = Corner_Invalid;
    {
        Vec2S32 n_p0 = V2S32(0, 0);
        Vec2S32 n_sz = atlas->rootSize;
        for(M_AtlasRegionNode *n = atlas->root, *next = 0; n != 0; n = next)
        {
            if(n_p0.x <= region.p0.x && region.p0.x < n_p0.x+n_sz.x &&
               n_p0.y <= region.p0.y && region.p0.y < n_p0.y+n_sz.y)
            {
                if(region.p0.x == n_p0.x && region.p0.y == n_p0.y && 
                   regionSize.x == n_sz.x && regionSize.y == n_sz.y)
                {
                    node = n;
                    calcRegionSize = n_sz;
                    break;
                }
                else
                {
                    Vec2S32 r_midpoint =
                    {
                        region.p0.x + regionSize.x/2,
                        region.p0.y + regionSize.y/2,
                    };
                    Vec2S32 n_midpoint =
                    {
                        n_p0.x + n_sz.x/2,
                        n_p0.y + n_sz.y/2,
                    };
                    Corner nextCorner = Corner_Invalid;
                    if(r_midpoint.x <= n_midpoint.x && r_midpoint.y <= n_midpoint.y)
                    {
                        nextCorner = Corner_00;
                    }
                    else if(r_midpoint.x <= n_midpoint.x && n_midpoint.y <= r_midpoint.y)
                    {
                        nextCorner = Corner_01;
                    }
                    else if(n_midpoint.x <= r_midpoint.x && r_midpoint.y <= n_midpoint.y)
                    {
                        nextCorner = Corner_10;
                    }
                    else if(n_midpoint.x <= r_midpoint.x && n_midpoint.y <= r_midpoint.y)
                    {
                        nextCorner = Corner_11;
                    }
                    next = n->children[nextCorner];
                    nodeCorner = nextCorner;
                    n_sz.x /= 2;
                    n_sz.y /= 2;
                    Vec2S32 sideVertex = SideVertexFromCorner32(nodeCorner);
                    n_p0.x += sideVertex.x*n_sz.x;
                    n_p0.y += sideVertex.y*n_sz.y;
                }
            }
            else
            {
                break;
            }
        }
    }
    
    //- Free node
    if((node != 0) && (nodeCorner != Corner_Invalid))
    {
        node->flags &= ~M_AtlasRegionNodeFlag_Taken;
        if(node->parent != 0)
        {
            node->parent->maxFreeSize[nodeCorner] = calcRegionSize;
        }
        for(M_AtlasRegionNode *p = node->parent; p != 0; p = p->parent)
        {
            p->numAllocatedDescendants -= 1;
            M_AtlasRegionNode *parent = p->parent;
            if(parent != 0)
            {
                Corner p_corner = (p == parent->children[Corner_00] ? Corner_00 :
                                   p == parent->children[Corner_01] ? Corner_01 :
                                   p == parent->children[Corner_10] ? Corner_10 :
                                   p == parent->children[Corner_11] ? Corner_11 :
                                   Corner_Invalid);
                if(p_corner == Corner_Invalid)
                {
                    InvalidPath;
                }
                parent->maxFreeSize[p_corner].x = Max(Max(p->maxFreeSize[Corner_00].x,
                                                          p->maxFreeSize[Corner_01].x),
                                                      Max(p->maxFreeSize[Corner_10].x,
                                                          p->maxFreeSize[Corner_11].x));
                parent->maxFreeSize[p_corner].y = Max(Max(p->maxFreeSize[Corner_00].y,
                                                          p->maxFreeSize[Corner_01].y),
                                                      Max(p->maxFreeSize[Corner_10].y,
                                                          p->maxFreeSize[Corner_11].y));
            }
        }
    }
}




////////////////////////////
//~ BOTTOM
#endif // #define BASE_MEMORY_INCLUDE_C