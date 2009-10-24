#ifndef __CORE_H__
#define __CORE_H__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

#include "Build.h"

#if RENDERING
#	include <SDL/SDL.h>		//?? move outside
#endif

#define VECTOR_ARG(name)	name[0],name[1],name[2]
#define QUAT_ARG(name)		name.x,name.y,name.z,name.w
#define ARRAY_ARG(array)	array, sizeof(array)/sizeof(array[0])
#define ARRAY_COUNT(array)	(sizeof(array)/sizeof(array[0]))

//?? move to UnCore.h ?
#define FVECTOR_ARG(v)		v.X, v.Y, v.Z
#define FQUAT_ARG(v)		v.X, v.Y, v.Z, v.W
#define FROTATOR_ARG(r)		r.Yaw, r.Pitch, r.Roll

#define BYTES4(a,b,c,d)	((a) | ((b)<<8) | ((c)<<16) | ((d)<<24))


#define assert(x)	\
	if (!(x))		\
	{				\
		appError("assertion failed: %s\n", #x); \
	}

// helper declararion
template<int> struct CompileTimeError;
template<>    struct CompileTimeError<true> {};

#define staticAssert(expr,name)			\
	{									\
		CompileTimeError<(expr) != 0> assert_##name; \
		(void)assert_##name;			\
	}

#undef M_PI
#define M_PI				(3.14159265358979323846)


#undef min
#undef max

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define bound(a,minval,maxval)  ( ((a) > (minval)) ? ( ((a) < (maxval)) ? (a) : (maxval) ) : (minval) )

#define appFloor(x)		( (int)floor(x) )
#define appCeil(x)		( (int)ceil(x) )
#define appRound(x)		( (int) (x >= 0 ? (x)+0.5f : (x)-0.5f) )


#if _MSC_VER
#	define vsnprintf		_vsnprintf
#	define FORCEINLINE		__forceinline
#	define NORETURN			__declspec(noreturn)
#	define GCC_PACK							// VC uses #pragma pack()
#	pragma warning(disable : 4291)			// no matched operator delete found
	// this functions are smaller, when in intrinsic form (and, of course, faster):
#	pragma intrinsic(memcpy, memset, memcmp, abs, fabs)
	// allow nested inline expansions
#	pragma inline_depth(8)
typedef __int64				int64;
#elif __GNUC__
#	define NORETURN			__attribute__((noreturn))
#	if (__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 2))
	// strange, but there is only way to work (inline+always_inline)
#		define FORCEINLINE	inline __attribute__((always_inline))
#	else
#		define FORCEINLINE	inline
#	endif
#	define stricmp			strcasecmp
#	define strnicmp			strncasecmp
#	define GCC_PACK			__attribute__((__packed__))
typedef signed long long	int64;
#else
#	error "Unsupported compiler"
#endif

// necessary types
typedef unsigned char		byte;
typedef unsigned short		word;


#define COLOR_ESCAPE	'^'		// may be used for quick location of color-processing code

#define S_BLACK			"^0"
#define S_RED			"^1"
#define S_GREEN			"^2"
#define S_YELLOW		"^3"
#define S_BLUE			"^4"
#define S_MAGENTA		"^5"
#define S_CYAN			"^6"
#define S_WHITE			"^7"


template<class T> inline T OffsetPointer(const T ptr, int offset)
{
	return (T) ((unsigned)ptr + offset);
}

// Align integer or pointer of any type
template<class T> inline T Align(const T ptr, int alignment)
{
	return (T) (((unsigned)ptr + alignment - 1) & ~(alignment - 1));
}

template<class T> inline void Exchange(T& A, T& B)
{
	const T tmp = A;
	A = B;
	B = tmp;
}

template<class T> inline void QSort(T* array, int count, int (*cmpFunc)(const T*, const T*))
{
	qsort(array, count, sizeof(T), (int (*)(const void*, const void*)) cmpFunc);
}

void appError(char *fmt, ...);
void* appMalloc(int size);
void appFree(void *ptr);

// log some interesting information
void appSetNotifyHeader(const char *fmt, ...);
void appNotify(char *fmt, ...);

const char *va(const char *format, ...);
int appSprintf(char *dest, int size, const char *fmt, ...);
void appStrncpyz(char *dst, const char *src, int count);
void appStrncpylwr(char *dst, const char *src, int count);
void appStrcatn(char *dst, int count, const char *src);
const char *appStristr(const char *s1, const char *s2);

void appMakeDirectory(const char *dirname);
void appMakeDirectoryForFile(const char *filename);


FORCEINLINE void* operator new(size_t size)
{
	return appMalloc(size);
}

FORCEINLINE void* operator new[](size_t size)
{
	return appMalloc(size);
}

FORCEINLINE void operator delete(void* ptr)
{
	appFree(ptr);
}

// inplace new
FORCEINLINE void* operator new(size_t size, void* ptr)
{
	return ptr;
}


#if DO_GUARD

// C++excpetion-based guard/unguard system
#define guard(func)						\
	{									\
		static const char __FUNC__[] = #func; \
		try {

#define unguard							\
		} catch (...) {					\
			appUnwindThrow(__FUNC__);	\
		}								\
	}

#define unguardf(msg)					\
		} catch (...) {					\
			appUnwindPrefix(__FUNC__);	\
			appUnwindThrow msg;			\
		}								\
	}

#define	THROW_AGAIN		throw
#define THROW			throw 1

void appUnwindPrefix(const char *fmt);		// not vararg (will display function name for unguardf only)
NORETURN void appUnwindThrow(const char *fmt, ...);

extern char GErrorHistory[2048];

#else  // DO_GUARD

#define guard(func)		{
#define unguard			}
#define unguardf(msg)	}

#endif // DO_GUARD


#define appMilliseconds()		SDL_GetTicks()

#if PROFILE
extern int GNumAllocs;
#endif


#include "Math3D.h"


#endif // __CORE_H__
