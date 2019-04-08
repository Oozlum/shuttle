#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include "lua.h"
#if 0
#ifdef __MINGW32__
/* some typedef functions - these are not matching exactly the
original definitions, but their signature is supposed to be
compatible
Currently 10 different functions are needed - each with different signatures... */
typedef void *voidfunc ();
typedef int *varfunc (void *L,...); // quick and dirty using the varargs
typedef void varfuncvoid (void *L,...);
typedef int (*lua_CFunction)(void *L);
#define LUA_GLOBALSINDEX (-10002)
#else
#include "lua.h"
#endif
#endif

/* create a Lua state, populate the bootstrap table with boot options and functions
 * and run the bootstrap code.
 */
void bootstrap(const char *osname, const char *arch, const char *cmd, const char *cwd, int argc, char **argv, lua_CFunction boot_msg);

#endif

