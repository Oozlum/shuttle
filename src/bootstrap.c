#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "romfs.h"
#include "bootstrap.h"

extern const char lua_bootstrap[];

#if 0
#ifdef __MINGW32__
extern voidfunc *luaL_newstate;
extern varfunc *luaL_loadstring;
extern varfunc *luaL_openlibs;
extern varfunc *lua_pcall;
extern varfunc *lua_pushcclosure;
extern varfunc *lua_setfield;
extern varfunc *lua_tolstring;
extern varfuncvoid *lua_createtable;
extern varfuncvoid *lua_pushstring;
#endif
#endif

/* Lua C function.  Takes a ROM blob and filename on the stack and returns the
 * file contents or nil.
 * Stack index 1: ROM string blob
 * Stack index 2: filename
 */
static int c_extract_romfile(lua_State *L)
{
  const char *rom, *file, *file_content;

  rom = luaL_checkstring(L, 1);
  file = luaL_checkstring(L, 2);
  file_content = extract_rom_file(rom, file);
  lua_settop(L, 0);

  if (file_content)
    lua_pushstring(L, file_content);
  else
    lua_pushnil(L);

  return 1;
}

/* create a Lua state, populate the bootstrap table with boot options and functions
 * and run the bootstrap code.
 */
void bootstrap(const char *osname, const char *arch, const char *cmd, const char *cwd, int argc, char **argv, lua_CFunction boot_msg)
{
  lua_State *L;
  const char *bootcode;
  int i;

  L = luaL_newstate();
  luaL_openlibs(L);

  lua_newtable(L);
  lua_pushcclosure(L, c_extract_romfile, 0);
  lua_setfield(L, -2, "extract_romfile");
  lua_pushcclosure(L, boot_msg, 0);
  lua_setfield(L, -2, "message");
  lua_pushstring(L, arch);
  lua_setfield(L, -2, "arch");
  lua_pushstring(L, osname);
  lua_setfield(L, -2, "osname");
  lua_pushstring(L, cmd);
  lua_setfield(L, -2, "cmd");
  lua_pushstring(L, cwd);
  lua_setfield(L, -2, "cwd");
  lua_setfield(L, LUA_GLOBALSINDEX, "bootstrap");

  /* load and run the bootstrap from ROM. */
  bootcode = extract_rom_file(lua_bootstrap, "bootstrap.lua");
  if (bootcode)
  {
    if (luaL_loadstring(L, bootcode) == 0)
    {
      for (i = 0; i != argc; ++i)
        lua_pushstring(L, argv[i]);

      if (lua_pcall(L, argc, LUA_MULTRET, 0) != 0)
        boot_msg(L);
    }
    else
      boot_msg(L);
  }
  else
  {
    lua_pushstring(L, "Failed to find bootstrap ROM!");
    boot_msg(L);
  }

  lua_settop(L, 0);
  lua_close(L);
}

