#ifndef BASE_CONTEXT_DETECTION_INCLUDE_H
#define BASE_CONTEXT_DETECTION_INCLUDE_H

// NOTE(gsp): file taken from rjf(Ryan Fleury)'s codebase

////////////////////////////////
//~ rjf: MSVC Extraction

#if defined(_MSC_VER)

# define COMPILER_MSVC 1

# if defined(_WIN32)
#  define OS_WINDOWS 1
# else
#  error _MSC_VER is defined, but _WIN32 is not. This setup is not supported.
# endif // #if defined(_WIN32)

# if defined(_M_AMD64)
#  define ARCH_X64 1
# elif defined(_M_IX86)
#  define ARCH_X86 1
# elif defined(_M_ARM64)
#  define ARCH_ARM64 1
# elif defined(_M_ARM)
#  define ARCH_ARM32 1
# else
#  error Target architecture is not supported. _MSC_VER is defined, but one of {_M_AMD64, _M_IX86, _M_ARM64, _M_ARM} is not.
# endif // #if defined(_M_AMD64)

# if _MSC_VER >= 1920
#  define COMPILER_MSVC_YEAR 2019
# elif _MSC_VER >= 1910
#  define COMPILER_MSVC_YEAR 2017
# elif _MSC_VER >= 1900
#  define COMPILER_MSVC_YEAR 2015
# elif _MSC_VER >= 1800
#  define COMPILER_MSVC_YEAR 2013
# elif _MSC_VER >= 1700
#  define COMPILER_MSVC_YEAR 2012
# elif _MSC_VER >= 1600
#  define COMPILER_MSVC_YEAR 2010
# elif _MSC_VER >= 1500
#  define COMPILER_MSVC_YEAR 2008
# elif _MSC_VER >= 1400
#  define COMPILER_MSVC_YEAR 2005
# else
#  define COMPILER_MSVC_YEAR 0
# endif

////////////////////////////////
//~ rjf: Clang Extraction

#elif defined(__clang__)

# define COMPILER_CLANG 1

# if defined(__APPLE__) && defined(__MACH__)
#  define OS_MAC 1
# elif defined(__gnu_linux__)
#  define OS_LINUX 1
# else
#  error __clang__ is defined, but one of {__APPLE__, __gnu_linux__} is not. This setup is not supported.
# endif

# if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#  define ARCH_X64 1
# elif defined(i386) || defined(__i386) || defined(__i386__)
#  define ARCH_X86 1
# elif defined(__aarch64__)
#  define ARCH_ARM64 1
# elif defined(__arm__)
#  define ARCH_ARM32 1
# else
#  error Target architecture is not supported. __clang__ is defined, but one of {__amd64__, __amd64, __x86_64__, __x86_64, i386, __i386, __i386__, __aarch64__, __arm__} is not.
# endif

////////////////////////////////
//~ gsp: Extraction error
#else
#  error Compiler is not supported. _MSC_VER, __clang__, __GNUC__, or __GNUG__ must be defined.
#endif

#if defined(ARCH_X64)
# define ARCH_64BIT 1
#elif defined(ARCH_X86)
# define ARCH_32BIT 1
#endif // #if defined(ARCH_X64)

////////////////////////////////
//~ gsp: Language

#if defined(__cplusplus)
# define LANG_CPP 1
#else
# define LANG_C 1
#endif

////////////////////////////////
//~ rjf: Zero

#if !defined(ARCH_32BIT)
# define ARCH_32BIT 0
#endif

#if !defined(ARCH_64BIT)
# define ARCH_64BIT 0
#endif

#if !defined(ARCH_X64)
# define ARCH_X64 0
#endif

#if !defined(ARCH_X86)
# define ARCH_X86 0
#endif

#if !defined(ARCH_ARM64)
# define ARCH_ARM64 0
#endif

#if !defined(ARCH_ARM32)
# define ARCH_ARM32 0
#endif

#if !defined(COMPILER_MSVC)
# define COMPILER_MSVC 0
#endif

#if !defined(COMPILER_GCC)
# define COMPILER_GCC 0
#endif

#if !defined(COMPILER_CLANG)
# define COMPILER_CLANG 0
#endif

#if !defined(OS_WINDOWS)
# define OS_WINDOWS 0
#endif

#if !defined(OS_LINUX)
# define OS_LINUX 0
#endif

#if !defined(OS_MAC)
# define OS_MAC 0
#endif

#if !defined(LANG_CPP)
# define LANG_CPP 0
#endif

#if !defined(LANG_C)
# define LANG_C 0
#endif

////////////////////////////////
//~ gsp: Build Parameters

#if !defined(BUILD_DEBUG)
# define BUILD_DEBUG 0
# pragma message("[CTX] Release build.")
#else
# pragma message("[CTX] Debug build.")
#endif

#if !defined(BUILD_CONSOLE_OUTPUT)
# define BUILD_CONSOLE_OUTPUT 0
#endif

#if !defined(BUILD_ROOT)
# define BUILD_ROOT 1
#endif

#if !defined(BUILD_EXTERNAL_USAGE)
# define BUILD_EXTERNAL_USAGE 0
#endif

#if !defined(BUILD_CORE_LINK_STATIC)
# define BUILD_CORE_LINK_STATIC 0 
#endif

////////////////////////////////
//~ End of file
#endif // BASE_CONTEXT_DETECTION_INCLUDE_H
