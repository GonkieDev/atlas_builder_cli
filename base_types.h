/* date = July 8th 2023 0:54 pm */

#ifndef BASE_TYPES_INCLUDE_H
#define BASE_TYPES_INCLUDE_H

#include <stdint.h>
#include <math.h>

#if COMPILER_MSVC
#include <intrin.h>
#endif

//- gsp: base types 
typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float  f32;
typedef double f64;

typedef i8  b8;
typedef i16 b16;
typedef i32 b32;
typedef i64 b64;

typedef void* VoidFunc;

//- gsp: keywords 

#define function static
#define global   static
#define local_persist static
#define inline_function inline

#define fallthrough

#if COMPILER_MSVC
# define _per_thread __declspec(thread)
#elif COMPILER_CLANG
# define _per_thread __thread
#elif COMPILER_GCC
# define _per_thread __thread
#else
# error "_per_thread must be defined! Something has gone wrong."
#endif

#define per_thread _per_thread

///////////////////////////////
//~ gsp: utils 

#define ArrayCount(arr) (sizeof(arr) / sizeof(*(arr)))

#define ProtectCall(call, success)    \
if ((success)) { if (!(call)) { (success) = 0; Assert(0); } }
#define ProtectAssign(assign, badTarget, success)    \
if ((success)) { if ((assign) == (badTarget)) { Assert(0); (success) = 0; } }

#define AbsoluteValueS32(x) (i32)abs((x))
#define AbsoluteValueS64(x) (i64)llabs((x))
#define AbsoluteValueU64(x) (u64)llabs((u64)(x))

//- gsp: Ptrs 
#define AlignPowOf2(x, p) (((x) + (p) - 1)&~((p) - 1))
#define IntFromPtr(p) (u64)((u8*)(p) - (u8*)0)
#define PtrFromInt(n) (void*)((u8*)0 + (n))

#define Swap(type, a, b) do{ type _swapper_ = a; a = b; b = _swapper_; }while(0)

#define Stringify_(S) #S
#define Stringify(S) Stringify_(S)
#define Glue_(A,B) A##B
#define Glue(A, B) Glue(A, B)

#define Member(T, m) (((T*)0)->m)
#define SizeOfMember(T, m) sizeof(Member(T, m))
#define OffsetOfMember(T, m) IntFromPtr(&Member(T, m))

//- gsp: Clamp, min, max 
#define Min(a, b) ( ((a)<(b)) ? (a) : (b) )
#define Max(a, b) ( ((a)>(b)) ? (a) : (b) )
#define Min3(a, b, c) (Min(Min(a, b), c))
#define Max3(a, b, c) (Max(Max(a, b), c))
#define Clamp(a, x, b) (((a)>(x))?(a):((b)<(x))?(b):(x))

// gsp: Size utils
#define KB(x) (x << 10)
#define MB(x) (x << 20)
#define GB(x) ((u64)x << 30)
#define TB(x) ((u64)x << 40)

// gsp: defer loop
#define DeferLoop(start, end) for(int _i_ = ((start), 0); _i_ == 0; _i_ += 1, (end))

//- Padding
#define PAD16(ammount) u16 padding16_x##ammount[ammount]
#define PAD32(ammount) u32 padding32_x##ammount[ammount]

//- gsp: Memory functions 

// dst, src, size
#define MemoryCopy memcpy
#define MemoryMove memmove
#define MemorySet  memset

#define MemoryCopyStruct(dst, src) do { Assert(sizeof(*(dst)) == sizeof(*(src))); MemoryCopy((dst), (src), sizeof(*(dst))); } while(0)
#define MemoryCopyArray(dst, src)  do { Assert(sizeof(dst) == sizeof(src)); MemoryCopy((dst), (src), sizeof(src)); }while(0)

#define MemoryZero(ptr, size) MemorySet((ptr), 0, (size))
#define MemoryZeroStruct(ptr) MemoryZero((ptr), sizeof(*(ptr)))
#define MemoryZeroArray(arr)  MemoryZero((arr), sizeof(arr))

#define UnusedVariable(name) (void)name

//- gsp: read_only 
#if OS_WINDOWS
# pragma section(".roglob", read)
# define read_only __declspec(allocate(".roglob"))
#else
# define read_only
#endif

//- gsp: Limits 
read_only global u8 U8Max = 0xFF;
read_only global u8 U8Min = 0;

read_only global u16 U16Max = 0xFFFF;
read_only global u16 U16Min = 0;

read_only global u32 U32Max = 0xFFFFFFFF;
read_only global u32 U32Min = 0;

read_only global u64 U64Max = 0xFFFFFFFFFFFFFFFF;
read_only global u64 U64Min = 0;

read_only global i8 I8Max = 0x7F;
read_only global i8 I8Min = -1 - 0x7F;

read_only global i16 I16Max = 0x7FFF;
read_only global i16 I16Min = -1 - 0x7FFF;

read_only global i32 I32Max = 0x7FFFFFFF;
read_only global i32 I32Min = -1 - 0x7FFFFFFF;

read_only global i64 I64Max = 0x7FFFFFFFFFFFFFFF;
read_only global i64 I64Min = -1 - 0x7FFFFFFFFFFFFFFF;

//- gsp: Constants 
read_only global f32 F32Max = (f32)(3.4028234664e+38);
read_only global f32 F32Min = (f32)(-3.4028234664e+38);
read_only global f32 F32SmallestPositive = (f32)(1.1754943508e-38);
read_only global f32 F32Epsilon = (f32)(5.96046448e-8);

read_only global u32 SignF32 = 0x80000000;
read_only global u32 ExponentF32 = 0x7F800000;
read_only global u32 MantissaF32 = 0x7FFFFF;

read_only global u64 SignF64 = 0x8000000000000000ull;
read_only global u64 ExponentF64 = 0x7FF0000000000000ull;
read_only global u64 MantissaF64 = 0xFFFFFFFFFFFFFull;

//- gsp: assertions 
#if OS_WINDOWS
#define BreakDebugger() __debugbreak()
#else
#define BreakDebugger() (*(volatile int *)0 = 0)
#endif

#undef Assert
#if BUILD_DEBUG
# define Assert(b) do { if(!(b)) { BreakDebugger(); } } while(0)
#else
# define Assert(b)
#endif

#define AssertMsg(b, msg) Assert((b) && (msg))

#define StaticAssert(b, msg) static_assert(b, msg)

#define NotImplemented AssertMsg(0, "Not Implemented")
#define InvalidPath AssertMsg(0, "Invalid Path")
#define NotImplementedStatic StaticAssert(0, "Not Implemented")
#define InvalidPathStatic StaticAssert(0, "Invalid Path")

#define UnreferencedParameter(param) ((void)(param))

//- gsp: Liked list stuff
#define CheckNull(p) ((p)==0)
#define SetNull(p) ((p)=0)

#define QueuePush_NZ(f,l,n,next,zchk,zset) (zchk(f)?\
(((f)=(l)=(n)), zset((n)->next)):\
((l)->next=(n),(l)=(n),zset((n)->next)))
#define QueuePushFront_NZ(f,l,n,next,zchk,zset) (zchk(f) ? (((f) = (l) = (n)), zset((n)->next)) :\
((n)->next = (f)), ((f) = (n)))
#define QueuePop_NZ(f,l,next,zset) ((f)==(l)?\
(zset(f),zset(l)):\
((f)=(f)->next))
#define StackPush_N(f,n,next) ((n)->next=(f),(f)=(n))
#define StackPop_NZ(f,next,zchk) (zchk(f)?0:((f)=(f)->next))

#define DLLInsert_NPZ(f,l,p,n,next,prev,zchk,zset) \
(zchk(f) ? (((f) = (l) = (n)), zset((n)->next), zset((n)->prev)) :\
zchk(p) ? (zset((n)->prev), (n)->next = (f), (zchk(f) ? (0) : ((f)->prev = (n))), (f) = (n)) :\
((zchk((p)->next) ? (0) : (((p)->next->prev) = (n))), (n)->next = (p)->next, (n)->prev = (p), (p)->next = (n),\
((p) == (l) ? (l) = (n) : (0))))
#define DLLPushBack_NPZ(f,l,n,next,prev,zchk,zset) DLLInsert_NPZ(f,l,l,n,next,prev,zchk,zset)
#define DLLRemove_NPZ(f,l,n,next,prev,zchk,zset) (((f)==(n))?\
((f)=(f)->next, (zchk(f) ? (zset(l)) : zset((f)->prev))):\
((l)==(n))?\
((l)=(l)->prev, (zchk(l) ? (zset(f)) : zset((l)->next))):\
((zchk((n)->next) ? (0) : ((n)->next->prev=(n)->prev)),\
(zchk((n)->prev) ? (0) : ((n)->prev->next=(n)->next))))


#define QueuePush(f,l,n)         QueuePush_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePushFront(f,l,n)    QueuePushFront_NZ(f,l,n,next,CheckNull,SetNull)
#define QueuePop(f,l)            QueuePop_NZ(f,l,next,SetNull)
#define StackPush(f,n)           StackPush_N(f,n,next)
#define StackPop(f)              StackPop_NZ(f,next,CheckNull)
#define DLLPushBack(f,l,n)       DLLPushBack_NPZ(f,l,n,next,prev,CheckNull,SetNull)
#define DLLPushFront(f,l,n)      DLLPushBack_NPZ(l,f,n,prev,next,CheckNull,SetNull)
#define DLLInsert(f,l,p,n)       DLLInsert_NPZ(f,l,p,n,next,prev,CheckNull,SetNull)
#define DLLRemove(f,l,n)         DLLRemove_NPZ(f,l,n,next,prev,CheckNull,SetNull)


///////////////////////////////////////////////
//~ End of file
#endif //BASE_TYPES_INCLUDE_H
