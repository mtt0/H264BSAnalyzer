#ifndef DEBUG_MEMORY_H_
#define DEBUG_MEMORY_H_

#if defined(__WIN32__)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <stdlib.h>

// This routine place comments at the head of a section of debug output
inline void OutputHeading(const char *explanation) {
  _RPT1(_CRT_WARN, "\n\n%s:\n**************************************\
************************************\n",
        explanation);
}

// The following macros set and clear, respectively, given bits
// of the C runtime library debug flag, as specified by a bitmask.
#ifdef _DEBUG
#define SET_CRT_DEBUG_FIELD(a)                                                 \
  _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define CLEAR_CRT_DEBUG_FIELD(a)                                               \
  _CrtSetDbgFlag(~(a)&_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define SET_CRT_DEBUG_FIELD(a) ((void)0)
#define CLEAR_CRT_DEBUG_FIELD(a) ((void)0)
#endif

#endif // 

#endif // DEBUG_MEMORY_H_
