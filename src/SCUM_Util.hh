#ifndef SCUM_UTIL_HH_INCLUDED
#define SCUM_UTIL_HH_INCLUDED

#include <new>

namespace SCUM
{
	static inline void CheckAlloc(void* ptr)
	{
		if (!ptr) throw std::bad_alloc();
	}
};

#ifdef SCUM_DEBUG
// unfortunately some SC headers define NDEBUG
// this hack makes assert usable again
# ifdef NDEBUG
#  define SCUM_NDEBUG_DEFINED NDEBUG
#  undef  NDEBUG
# endif

# include <assert.h>
# include <stdarg.h>
# include <stdio.h>

namespace SCUM
{
	static inline void DebugPrint(FILE* ostream, const char* file, int line, const char* func, const char* fmt, ...)
	{
		va_list ap;
		va_start(ap, fmt);
		fprintf(ostream, "%s:%d:%s ", file, line, func);
		vfprintf(ostream, fmt, ap);
		fflush(ostream);
		va_end(ap);
	}
};

# define SCUM_ASSERT(expr) \
		assert(expr)
# define SCUM_ASSERT_PTR(ptr) \
		SCUM_ASSERT((void*)(ptr) != (void*)0)
# define SCUM_DEBUG_PRINT(...) \
		SCUM::DebugPrint(stderr, __FILE__, __LINE__, __func__, __VA_ARGS__)

# ifdef SCUM_NDEBUG_DEFINED
#  define NDEBUG SCUM_NDEBUG_DEFINED
#  undef  SCUM_NDEBUG_DEFINED
# endif
#else // !SCUM_DEBUG
# define SCUM_ASSERT(expr) /* SCUM_ASSERT */
# define SCUM_DEBUG_PRINT(fmt, ...) /* SCUM_DEBUG_PRINT */
#endif // SCUM_DEBUG

#endif // SCUM_UTIL_HH_INCLUDED
