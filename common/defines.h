// This file is an ever growing collection of type definitions,
// macros and functions that can be usefull in every part of the code
// NOTE(valentino): The only compiler supported is MSVC

#ifndef DEFINES_H
#define DEFINES_H

#define Assert(Condition) \
    if (!(Condition))     \
    __debugbreak()

#define BreakHere Assert(false)

#define FORCEINLINE __forceinline
#define FORBIDINLINE __declspec(noinline)

#include "stdint.h"

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef u8 byte;
typedef u16 word;
typedef u32 dword;
typedef u64 qword;

typedef i8 b8;
typedef i32 b32;

#define local_persist static
#define global_variable static
#define internal_func static

#define ArrayCount(array) (sizeof(array) / sizeof((array)[0]))

#define KiB(value) ((value) * 1024LL)
#define MiB(value) (KiB(value) * 1024LL)
#define GiB(value) (MiB(value) * 1024LL)
#define TiB(value) (GiB(value) * 1024LL)

#define CheckFlags(Flags, FlagsToCheck) ((Flags) & (FlagsToCheck))

template <typename T>
FORCEINLINE T Max(T A, T B)
{
    return A > B ? A : B;
}
template <typename T>
FORCEINLINE T Min(T A, T B)
{
    return A < B ? A : B;
}
#define Clamp(X, Low, High) Min(High, Max(Low, X))

#endif