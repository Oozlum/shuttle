// this is an extremly ugly quick and dirty hack...
// maybe it could be refactored to do some error catching and
// other things, but right now it does what it should...
// (providing a single exe file in our main directory without
// polluting it with all these dlls located in the /bin folder)

#ifdef __MINGW32__
#define _WIN32_WINNT 0x0502
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winbase.h>
#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "bootstrap.h"

#ifndef OSNAME
#define OSNAME "Windows"
#endif

#ifndef ARCHNAME
#if defined WIN32
#define ARCHNAME "i386"
#else
#define ARCHNAME "x86_64"
#endif
#endif

#if 0
voidfunc *luaL_newstate;
varfunc *luaL_loadbuffer;
varfunc *luaL_openlibs;
varfunc *lua_pcall;
varfunc *lua_pushcclosure;
varfunc *lua_setfield;
varfunc *lua_tolstring;
varfuncvoid *lua_createtable;
varfuncvoid *lua_pushstring;
#endif

static int boot_msg(lua_State *L)
{
  const char *msg = luaL_checkstring(L, -1);

  MessageBox(NULL, msg, "Message", MB_OK|MB_ICONERROR);

  return 0;
}

PCHAR* CommandLineToArgv(PCHAR CmdLine,int* _argc)
{
  PCHAR* argv;
  PCHAR  _argv;
  size_t  len;
  ULONG   argc;
  CHAR   a;
  size_t   i, j;

  BOOLEAN  in_QM;
  BOOLEAN  in_TEXT;
  BOOLEAN  in_SPACE;

  len = strlen(CmdLine);
  i = ((len+2)/2)*sizeof(PVOID) + sizeof(PVOID);

  argv = (PCHAR*)GlobalAlloc(GMEM_FIXED, i + (len+2)*sizeof(CHAR));

  _argv = (PCHAR)(((PUCHAR)argv)+i);

  argc = 0;
  argv[argc] = _argv;
  in_QM = 0;
  in_TEXT = 0;
  in_SPACE = 1;
  i = 0;
  j = 0;

  while( a = CmdLine[i] ) {
    if(in_QM) {
      if(a == '\"') {
        in_QM = 0;
      } else {
        _argv[j] = a;
        j++;
      }
    } else {
      switch(a) {
        case '\"':
          in_QM = 1;
          in_TEXT = 1;
          if(in_SPACE) {
            argv[argc] = _argv+j;
            argc++;
          }
          in_SPACE = 0;
          break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
          if(in_TEXT) {
            _argv[j] = '\0';
            j++;
          }
          in_TEXT = 0;
          in_SPACE = 1;
          break;
        default:
          in_TEXT = 1;
          if(in_SPACE) {
            argv[argc] = _argv+j;
            argc++;
          }
          _argv[j] = a;
          j++;
          in_SPACE = 0;
          break;
      }
    }
    i++;
  }
  _argv[j] = '\0';
  argv[argc] = NULL;

  (*_argc) = argc;
  return argv;
}

PCHAR WideCharToUTF8(LPCWSTR text) {
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);
  PCHAR buffer = (PCHAR)GlobalAlloc(GMEM_FIXED, size_needed);
  WideCharToMultiByte(CP_UTF8, 0, text, -1, buffer, size_needed, NULL, NULL);
  return buffer;
}

int WINAPI WinMain(HINSTANCE hInstance,  HINSTANCE hPrevInstance,  LPSTR lpCmdLine, int nCmdShow)
{
  int argc;
  char ** argv = CommandLineToArgv(WideCharToUTF8(GetCommandLineW()),&argc);
  const char *cmd;
  HINSTANCE hinstLib;
  char buffer[MAX_PATH],*file;

  cmd = argv[0];
  ++argv;
  --argc;

  if (!GetFullPathName(cmd,MAX_PATH,buffer,&file)) {
    MessageBox(NULL,
      TEXT("Couldn't find the executable path"),
      TEXT("Failed to start"),
      MB_OK|MB_ICONERROR);
    return 0;
  }
  if (file!=NULL) *file = 0; // finish the string, don't need the appname

  LPWSTR path = (LPWSTR)GlobalAlloc(GMEM_FIXED, MAX_PATH+1);
  if (GetCurrentDirectoryW(MAX_PATH, path) == 0) {
    MessageBox(NULL,
      TEXT("Couldn't find the current working directory"),
      TEXT("Failed to start"),
      MB_OK|MB_ICONERROR);
    return 0;
  }

  SetCurrentDirectory(buffer);

/*
  hinstLib = LoadLibrary(".\\lua51.dll");
  if (hinstLib != NULL) {
    luaL_newstate = (voidfunc*) GetProcAddress(hinstLib, "luaL_newstate");
    luaL_loadbuffer = (varfunc*) GetProcAddress(hinstLib, "luaL_loadbuffer");
    luaL_openlibs = (varfunc*) GetProcAddress(hinstLib, "luaL_openlibs");
    lua_pcall = (varfunc*)GetProcAddress(hinstLib, "lua_pcall");
    lua_tolstring = (varfunc*)GetProcAddress(hinstLib, "lua_tolstring");
    lua_setfield = (varfunc*)GetProcAddress(hinstLib, "lua_setfield");
    lua_pushcclosure = (varfunc*)GetProcAddress(hinstLib, "lua_pushcclosure");
    lua_createtable = (varfuncvoid*)GetProcAddress(hinstLib, "lua_createtable");
    lua_pushstring = (varfuncvoid*)GetProcAddress(hinstLib, "lua_pushstring");
    // If the function address is valid, call the function.

    if (luaL_newstate && luaL_loadbuffer && luaL_openlibs && lua_pcall &&
      lua_pushcclosure && lua_setfield && lua_tolstring &&
      lua_createtable && lua_pushstring)
    {
        lua_pushstring(L,WideCharToUTF8(path));
    } else {
      MessageBox(NULL,
        TEXT("Could not load all functions that are supposed to be located in lua51.dll."),
        TEXT("Failed to start"),
        MB_OK|MB_ICONERROR);
    }

    // Free the DLL module.
    FreeLibrary(hinstLib);
  } else {
    MessageBox(NULL,
      TEXT("lua51.dll could not be found or loaded, please check the working directory of the application."),
      TEXT("Failed to initialize"),
      MB_OK|MB_ICONERROR);
  }
*/

  bootstrap(OSNAME, ARCHNAME, cmd, WideCharToUTF8(path), argc, argv, boot_msg);

  return 0;
}
