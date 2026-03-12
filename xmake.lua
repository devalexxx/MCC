set_project("MachinaCubicaCatalyst")
set_version("0.0.1")
set_languages("c++23")

includes("xmake/**.lua")

add_rules("mode.debug", "mode.release", "mode.releasedbg")

option("shared", { description = "", default = false })

add_repositories("devalexxx-repo https://github.com/devalexxx/xmake-repo.git")

local function merge(t1, t2)
    local result = {}
    for k,v in pairs(t1) do result[k] = v end
    for k,v in pairs(t2) do result[k] = v end
    return result
end

local usage_all = { ["MCCClient"] = true, ["MCCCommon"] = true, ["MCCServer"] = true }
local packages = {
    {
        name  = "fmt",
        extra = " >= 11",
        usage = usage_all,
        rconf = {}
    },
    { 
        name  = "hexis", 
        extra = "",
        usage = usage_all,
        rconf = { math = true }
    }, 
    { 
        name  = "flecs", 
        extra = "", -- " 4.1.0""
        usage = usage_all,
        rconf = {}-- { cxflags = "-DFLECS_CPP_NO_AUTO_REGISTRATION=1" }
    }, 
    { 
        name  = "glfw", 
        extra = "",
        usage = { ["MCCClient"] = true },
        rconf = {}
    }, 
    { 
        name  = "glad",
        extra = "",
        usage = { ["MCCClient"] = true },
        rconf = {}
    },
    {
        name  = "stb",
        extra = "",
        usage = { ["MCCClient"] = true },
        rconf = {}
    },
    {
        name  = "imgui",
        extra = "",
        usage = { ["MCCClient"] = true },
        rconf = { glfw = true, opengl3 = true }
    },
    { 
        name = "glm",
        extra = "",
        usage = usage_all,
        rconf = {}
    }, 
    { 
        name = "enet",
        extra = "",
        usage = usage_all,
        rconf = {}
    },
    { 
        name = "cereal",
        extra = "",
        usage = usage_all,
        rconf = {}
    }, 
    { 
        name = "perlinnoise",
        extra = "",
        usage = { ["MCCServer"] = true },
        rconf = {}
    }, 
    { 
        name = "zlib",
        extra = "",
        usage = usage_all,
        rconf = {}
    } 
}
for _, package in ipairs(packages) do
    add_requires(
        package.name .. package.extra, 
        { 
            debug   = is_mode("debug"),
            configs = merge({ shared = has_config("shared") }, package.rconf)
        }
    )
end

if is_host("macos") or is_host("linux") then
    add_defines("MCC_POSIX")
end

if is_host("windows") then
    add_cxxflags("cl::/Zc:preprocessor")
end

if is_mode("debug") then 
    add_defines("MCC_DEBUG")
    set_warnings("allextra", "error")
    -- set_warnings("allextra")
end

if is_mode("releasedbg") then
    add_defines("MCC_LOG_LEVEL=MCC_DEBUG_LEVEL")
    add_defines("MCC_BENCH")
    add_defines("MCC_RELEASE_DEBUG")
end

add_defines("GLM_ENABLE_EXPERIMENTAL")
add_defines("FLECS_CPP_NO_AUTO_REGISTRATION")

target("MCCCommon")
    add_defines("MCC_LIB_BUILD")

    if has_config("shared") then
        set_kind("shared")
        add_defines("MCC_LIB_SHARED")
    else
        set_kind("static")
    end

    add_includedirs("Common/include/",  { public = true })
    add_headerfiles("Common/include/(**/*.h)", "Common/include/(**/*.inl)")

    add_files("Common/**/*.cpp")

    for _, package in pairs(packages) do 
        if package.usage["MCCCommon"] then
            add_packages(package.name)
        end
    end

target("MCCClient")
    set_kind("binary")

    -- Use to don't care about glfw and glad include order
    add_defines("GLFW_INCLUDE_NONE")

    -- TODO: replace by runtime asset path
    on_load(function (target)
        os.cp("$(projectdir)/Assets/*", "$(builddir)/$(plat)/$(arch)/$(mode)/Assets/")
    end)

    add_includedirs("Client/include/",  { public = true })
    add_headerfiles("Client/include/(**/*.h)", "Client/include/(**/*.inl)")

    add_files("Client/**/*.cpp")

    if is_plat("macosx") then
        add_defines("MCC_MACOSX")
    end

    add_deps("MCCCommon")
    for _, package in pairs(packages) do 
        if package.usage["MCCClient"] then
            add_packages(package.name)
        end
    end

target("MCCServer")
    set_kind("binary")

    add_includedirs("Server/include/")
    add_headerfiles("Server/include/(**/*.h)", "Server/include/(**/*.inl)")

    add_files("Server/**/*.cpp")

    add_deps("MCCCommon")
    for _, package in pairs(packages) do 
        if package.usage["MCCServer"] then
            add_packages(package.name)
        end
    end