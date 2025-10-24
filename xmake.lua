-- Project settings
set_project("threadlet")
set_version("1.0.0")
set_languages("cxx17")

-- Build modes
add_rules("mode.debug", "mode.release")

-- Dependencies
add_requires("conan::stduuid/1.2.3", {alias = "stduuid"})
add_requires("conan::spdlog/1.16.0", {alias = "spdlog"})
add_requires("conan::doctest/2.4.12", {alias = "doctest"})

-- Library
target("threadlet")
    set_kind("headeronly")
    set_basename("threadlet")

    -- Headers files
    add_includedirs("include", {public = true})
    add_headerfiles("include/(**.h)")
    
    -- Link dependencies
    add_packages("stduuid", "spdlog")

    -- Threading support (POSIX)
    if is_plat("linux", "macosx") then
        add_syslinks("pthread")
    end

    -- Build modes
    if is_mode("debug") then
        set_symbols("debug")
        set_optimize("none")
    else
        set_symbols("hidden")
        set_optimize("fastest")
    end
target_end()

-- Unit tests
target("threadlet_tests")
    set_kind("binary")
    add_deps("threadlet")

    add_files("tests/**.cpp")

    add_packages("stduuid", "spdlog", "doctest")

    set_rundir("$(projectdir)/tests")
target_end()

-- Cleanup task
task("clean")
    on_run(function ()
        os.rm("build")
    end)
