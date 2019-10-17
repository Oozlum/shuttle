MKROM=../tools/mkrom
CC=gcc

LUA=luajit
LUAINC=../${LUA}/include

default:
	echo make osx, win, linux_x86 or linux_x64

osx: bootstrap_rom
	$(eval LUABINDIR=../${LUA}/osx_x64/Shuttle.app/Contents/Shuttle/)
	${CC} -o ${LUABINDIR}/shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUABINDIR} -llua -pagezero_size 10000 -image_base 100000000
	install_name_tool -change liblua.dylib @rpath/liblua.dylib ${LUABINDIR}/shuttle
	install_name_tool -add_rpath @executable_path/. ${LUABINDIR}/shuttle

win: bootstrap_rom
	$(eval LUABINDIR=../${LUA}/win_x86/Shuttle.app/Contents/Shuttle/)
	${CC} -mwindows -o ${LUABINDIR}/shuttle win_main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUABINDIR} -llua51

linux_x86: bootstrap_rom
	$(eval LUABINDIR=../${LUA}/$@/)
	${CC} -m32 -o ${LUABINDIR}/shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUABINDIR} -l:libluajit-5.1.so.2 -Wl,-rpath,'$$ORIGIN'

linux_x64: bootstrap_rom
	LUABINDIR=../${LUA}/$@/
	${CC} -o ${LUABINDIR}/shuttle main.c bootstrap.c lua_bootstrap.c romfs.c -I${LUAINC} -L${LUABINDIR} -l:libluajit-5.1.so.2 -Wl,-rpath,'$$ORIGIN'

bootstrap_rom: ${MKROM}
	${MKROM} c bootstrap_rom/ lua_bootstrap lua_bootstrap.c bootstrap_rom/

clean:
	rm -f lua_bootstrap.c
