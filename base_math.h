#ifndef BASE_MATH_H
#define BASE_MATH_H

typedef enum Corner
{
    Corner_Invalid = -1,
    Corner_00,
    Corner_01,
    Corner_10,
    Corner_11,
    Corner_COUNT
}
Corner;
#define EachCorner(name) Corner name = (Corner)0; name < Corner_COUNT; name = (Corner)(name+1)

typedef struct Vec2S32 Vec2S32;
union Vec2S32
{
    i32 x;
    i32 y;
};

typedef union Rng2S32 Rng2S32;
union Rng2S32
{
    struct
    {
        Vec2S32 min;
        Vec2S32 max;
    };
    struct
    {
        Vec2S32 p0;
        Vec2S32 p1;
    };
};

////////////////////////////////////////////////////////////////////////////////////
//~

inline_function Vec2S32 V2S32(i32 x, i32 y) { Vec2S32 result = {x,y}; return result; }
inline_function Vec2S32 Add2S32(Vec2S32 a, Vec2S32 b) { return V2S32(a.x + b.x, a.y + b.y); }
inline_function Vec2S32 Dim2S32(Rng2S32 r) { return V2S32(AbsoluteValueS32(r.max.x - r.min.x), AbsoluteValueS32(r.max.y - r.min.y)); }

function Vec2S32
SideVertexFromCorner32(Corner corner)
{
    Vec2S32 result = {0};
    switch(corner)
    {
        default: break;
        case Corner_00:{result = V2S32(0, 0);}break;
        case Corner_01:{result = V2S32(0, 1);}break;
        case Corner_10:{result = V2S32(1, 0);}break;
        case Corner_11:{result = V2S32(1, 1);}break;
    }
    return result;
}



////////////////////////////////////////////////////////////////////////////////////
//~ End of file
#endif //BASE_MATH_H
