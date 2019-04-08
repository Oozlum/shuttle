#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "lua.h"
#include "lauxlib.h"
#include "bootstrap.h"

#ifndef OSNAME
#ifdef __APPLE__
#define OSNAME "Mac"
#else
#define OSNAME "Unix"
#endif
#endif

#ifndef ARCHNAME
#if defined __x86_64
#define ARCHNAME "x86_64"
#elif defined __i386
#define ARCHNAME "i386"
#endif
#endif

static int boot_msg(lua_State *L)
{
  const char *msg;

  msg = luaL_checkstring(L, -1);
  printf("%s\n", msg);

  return 0;
}

int main(int argc, char **argv)
{
  char cwd_buf[256];
  const char *cwd, *appname;

  getcwd(cwd_buf, sizeof(cwd_buf));
  cwd = cwd_buf;

  /* remove the appname from the argument list. */
  appname = argv[0];
  --argc;
  ++argv;

  /* check for a -cwd argument and remove it from the list. */
  if (argc >= 1 && strcmp(argv[0], "-cwd") == 0)
  {
    --argc;
    ++argv;
    if (argc >= 1)
    {
      cwd = argv[0];
      --argc;
      ++argv;
    }
  }

  /* call the boostrap code. */
  bootstrap(OSNAME, ARCHNAME, appname, cwd, argc, argv, boot_msg);

  return 0;
}

