MKROM=../tools/mkrom
CC=gcc

LUA=luajit
LUAINC=../${LUA}/include

default:
	echo make osx_x86, osx_64, win, linux_x86 or linux_x64

osx_x86: bootstrap_rom
	$(eval LUALIBDIR=../${LUA}/osx_i386/Shuttle.app/Contents/Shuttle/)
	${CC} -m32 -o shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUALIBDIR} -llua

osx_x64: bootstrap_rom
	$(eval LUALIBDIR=../${LUA}/osx_i386/Shuttle.app/Contents/Shuttle/)
	${CC} -m32 -o shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUALIBDIR} -llua

win: bootstrap_rom
	${CC} -mwindows -o shuttle win_main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUALIBDIR} -llua51

linux_x86: bootstrap_rom
	$(eval LUALIBDIR=../${LUA}/$@/)
	${CC} -m32 -o shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUALIBDIR} -l:libluajit-5.1.so.2 -Wl,-rpath,'$$ORIGIN'

linux_x64: bootstrap_rom
	LUALIBDIR=../${LUA}/$@/
	${CC} -o shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUALIBDIR} -l:libluajit-5.1.so.2 -Wl,-rpath,'$$ORIGIN'

bootstrap_rom: ${MKROM}
	${MKROM} c bootstrap_rom/ lua_bootstrap lua_bootstrap.c bootstrap_rom/

clean:
	rm -f shuttle shuttle.exe lua_bootstrap.c
