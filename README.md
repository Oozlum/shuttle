# shuttle
shuttle is a cross-platform mechanism for packaging and launching Lua applications.  It provides skeleton directory structures and compiled binaries for each supported Lua version, O/S and architecture.  The intention is that you should be able to package your Lua application as a double-clickable runtime simply by copying your Lua source files, or source image, into the skeleton.

shuttle was inspired by the way [ZeroBraneStudio](https://github.com/pkulchenko/ZeroBraneStudio) launches itself.

## Skeleton directory structure
The packaging structure is as follows:

    .
    |-- shuttle[.exe]
    |-- liblua.so (or equivalent)
    |-- src/
        |-- <application source files>
    |-- app.bin (application source archive)
    |-- lua/
        |-- <lua libraries>
    |-- lib/
        |-- <lua compiled libraries>
    
## Bootstrapping process
The `shuttle` executable performs some basic bootstrapping before launching the Lua application.  The process is as follows:
1. set `package.path` to find files in `src/` then in `lua/`
2. set `package.cpath` to find binaries in `lib/`, formatted correctly for the host system
3. implement a custom `package.loader` to find `src/` files within `app.bin`
4. override `dofile()` and `loadfile()` to search `app.bin` for `src/` files
5. executes `dofile('src/app.lua')`
6. removes the bootstrapping and restores the environment
7. calls `main(...)`

## Application source files
Sources files can be stored individually under `src/` or packaged as an archive in `app.bin`.  A tool `mkrom` is provided to generate the archive from a directory of files.  Files found in `src/` on disk will override those in `app.bin`.

