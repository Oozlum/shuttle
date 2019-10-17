#!/bin/sh

ZBS_GIT_DIR=~/src/git/ZeroBraneStudio

if [ -z $ZBS_GIT_DIR ]; then
  ZBS_GIT_DIR=$1
fi

if [ -z $ZBS_GIT_DIR ]; then
  echo "ZBS_GIT_DIR not set"
  exit 0
fi

MAC_DIR=./luajit/osx_x64/Shuttle.app/Contents/Shuttle
WIN_DIR=./luajit/win_x86
LINUX32_DIR=./luajit/linux_x86
LINUX64_DIR=./luajit/linux_x64

# copy LuaJIT binaries and libraries from ZBS
# Base binaries
cp ${ZBS_GIT_DIR}/bin/liblua.dylib ${MAC_DIR}
cp ${ZBS_GIT_DIR}/bin/lua51.dll ${WIN_DIR}

for file in libssl libcrypto; do
  cp ${ZBS_GIT_DIR}/bin/${file}*.dylib ${MAC_DIR}
  cp ${ZBS_GIT_DIR}/bin/${file}*.dll ${WIN_DIR}
  cp ${ZBS_GIT_DIR}/bin/linux/x86/${file}*.so.* ${LINUX32_DIR}
  cp ${ZBS_GIT_DIR}/bin/linux/x64/${file}*.so.* ${LINUX64_DIR}
done

# Lua C libraries
for dir in mime socket git; do
  mkdir -p ${MAC_DIR}/libs/${dir}
  mkdir -p ${WIN_DIR}/libs/${dir}
  mkdir -p ${LINUX32_DIR}/libs/${dir}
  mkdir -p ${LINUX64_DIR}/libs/${dir}
done

for file in lfs lpeg ssl lexlpeg git/core mime/core socket/core; do
  cp ${ZBS_GIT_DIR}/bin/clibs/${file}.dylib ${MAC_DIR}/libs/${file}.dylib
  cp ${ZBS_GIT_DIR}/bin/clibs/${file}.dll ${WIN_DIR}/libs/${file}.dll
  cp ${ZBS_GIT_DIR}/bin/linux/x86/clibs/${file}.so ${LINUX32_DIR}/libs/${file}.so
  cp ${ZBS_GIT_DIR}/bin/linux/x64/clibs/${file}.so ${LINUX64_DIR}/libs/${file}.so
done

cp ${ZBS_GIT_DIR}/bin/clibs/libwx.dylib ${MAC_DIR}/libs
cp ${ZBS_GIT_DIR}/bin/clibs/wx.dll ${WIN_DIR}/libs
cp ${ZBS_GIT_DIR}/bin/linux/x86/clibs/libwx.so ${LINUX32_DIR}/libs
cp ${ZBS_GIT_DIR}/bin/linux/x64/clibs/libwx.so ${LINUX64_DIR}/libs

for file in libzlib.dll winapi.dll; do
  cp ${ZBS_GIT_DIR}/bin/clibs/${file} ${WIN_DIR}/libs/${file}
done

cp -r ${ZBS_GIT_DIR}/lualibs ${MAC_DIR}/lua
cp -r ${ZBS_GIT_DIR}/lualibs ${WIN_DIR}/lua
cp -r ${ZBS_GIT_DIR}/lualibs ${LINUX32_DIR}/lua
cp -r ${ZBS_GIT_DIR}/lualibs ${LINUX64_DIR}/lua

