-- Bootstrap Lua file.
--
-- 1. Set the Lua module paths.
-- 2. Try to load the app.bin ROM file and if successful, modify loadfile, dofile and
--      package.loaders to load from ROM if the disk file doesn't exist.
-- 3. Call the application entry point 'src/app.lua'
-- 4. Restore the environment and remove all bootstrapping.
-- 5. Call the application's main() function

-- the bootstrap table must be created by the caller
-- bootstrap.extract_romfile(...) populated by the caller.
bootstrap.dofile = dofile
bootstrap.loadfile = loadfile
bootstrap.path = package.path
bootstrap.cpath = package.cpath

-- don't search the ROM for standard Lua libraries
package.rompath = 'src/?.lua;src/?/?.lua;src/?/init.lua;'
package.path = package.rompath .. 'lua/?.lua;lua/?/?.lua;lua/?/init.lua;'

if bootstrap.osname == 'Windows' then
  package.cpath = 'lib/?.dll'
elseif bootstrap.osname == 'Mac' then
  package.cpath = 'lib/lib?.dylib;lib/?.dylib'
else
  package.cpath = 'lib/lib?.so;lib/?.so'
end

-- bootstrap.load_romfile is used by the package/script load functions and just
-- calls bootstrap.extract_romfile.  The purpose is to make it easy to overload
-- to pre-process the file, for example to decompress it.
bootstrap.load_romfile = function(...)
  return bootstrap.extract_romfile(...)
end

-- try to load the ROM file.
local f = io.open('app.bin', 'rb')
if f then
  bootstrap.rom = f:read('*a')
  f:close()
  f = nil

  local function file_not_found(err)
    if err:match('No such file or directory') then
      return true
    end
    return false
  end

  loadfile = function(file)
    local f, err = bootstrap.loadfile(file)
    if not f then
      if file_not_found(err) then
        f = bootstrap.load_romfile(bootstrap.rom, file)
        if f then
          f, err = loadstring(f, file)
        else
          f, err = nil, 'cannot open ' .. tostring(file) .. ': No such file or directory'
        end
      end
    end

    return f, err
  end

  dofile = function(file)
    local f, err = loadfile(file)
    if not f then
      error(err)
    end
    return f()
  end

  table.insert(package.loaders, 3, function(modulename)
    local modulepath = string.gsub(modulename, "%.", "/")
    for path in string.gmatch(package.rompath, "([^;]+)") do
      local filename = string.gsub(path, "%?", modulepath)
      local func, err = loadfile(filename)
      if func then
        return func
      elseif not file_not_found(err) then
        assert(false, err)
      end
    end
    return nil
  end)
end

-- call the application's entry point 'app.lua'
local main = dofile('src/app.lua', ...)

-- get rid of the bootstrapping and restore the environment.
bootstrap.rom = nil
dofile = bootstrap.dofile
loadfile = bootstrap.loadfile
package.path = bootstrap.path
package.cpath = bootstrap.cpath
bootstrap = nil

-- finally, call the application's main function, if it exists.
main = main or _G.main
if type(main) == 'function' then
  main()
end

os.exit()

