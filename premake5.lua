dofile("./scripts/ext.lua")

function replaceChars(str, ch, with)
    local len = str:length()
    do
        local t = type(ch)
        assert(t ~= "table", "'ch' cannot be a table!")
        if t == "number" then
            ch = string.format("%c", ch)
        end

        if string.length(ch) > 1 then
            ch = ch:char(1)
        end
    end

    local retval = ""

    for i=1,len,1 do
        local c = str:char(i)
        if c == ch then
            retval = retval .. with
        else
            retval = retval .. c
        end
    end

    return retval
end

function removeIfExists(path)
    if os.isfile(path) then
        os.remove(path)
        print("Removed file: " .. path)
    elseif os.isdir(path) then
        os.rmdir(path)
        print("Removed dir: " .. path)
    else
        --print("Not removing " .. path)
    end
end

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
srcdir = "%{prj.location}/src"

targetdir_prefix = ("bin/" .. outputdir)
objdir_prefix = ("bin-int/" .. outputdir)

newaction {
    trigger = "clean",
    description = "Clean the workspace",
    execute = function()
        local _dirs = {}
        local _count
        _dirs[1] = "."
        _count = #_dirs
        for i=1,_count,1 do
            local _path = _dirs[i]
            removeIfExists(path.join(_path, "Makefile"))
            removeIfExists(path.join(_path, "bin"))
            removeIfExists(path.join(_path, "bin-int"))
            removeIfExists(path.join(_path, "ddb.make"))
        end
        print("finished cleaning the project")
    end
}

newaction {
    trigger = "newheader",
    description = "Writes a new header",
    execute = _loadfile("./scripts/action_newheader.lua")
}

workspace "Dewpsi Database"
    configurations { "Debug", "Release" }

project "ddb"
    kind "SharedLib"
    language "C"
    defines {
        "USE_TCL_STUBS"
    }
    architecture "x64"
    flags "MultiProcessorCompile"
    links {
        "tclstub8.6"
    }
    files {
        "generic/*.c",
        "include/**.h"
    }
    targetdir (targetdir_prefix)
    objdir (objdir_prefix)
    includedirs {
        "include"
    }

-- Platform
filter "system:windows"
    defines "DDB_EXPORT_DLL"

-- Compiler
filter "toolset:gcc"
    cdialect "gnu11"

filter "not toolset:gcc"
    cdialect "C11"

filter "configurations:Debug"
    defines {
        "DDB_DEBUG",
        "TCL_MEM_DEBUG"
    }
    symbols "on"

filter "configurations:Release"
    defines {
        "PD_RELEASE"
    }
    optimize "on"
