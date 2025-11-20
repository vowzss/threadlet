-- project metadata
set_project("threadlet")
set_version("1.0.0")
set_languages("cxx17")

-- build modes
add_rules("mode.debug", "mode.release")

-- build settings
if is_mode("debug") then
    set_symbols("debug")
    set_optimize("none")
else
    set_symbols("hidden")
    set_optimize("fastest")
end

-- dependencies
add_requires("conan::stduuid/1.2.3", {alias = "stduuid"})
add_requires("conan::spdlog/1.16.0", {alias = "spdlog"})
add_requires("conan::doctest/2.4.12", {alias = "doctest"})

-- core target
target("threadlet")
    set_kind("headeronly")
    set_filename("threadlet")

    -- headers
    add_headerfiles("include/threadlet/(**.h)")
    add_includedirs("include", {public = true})
    
    -- sources
    add_packages("stduuid", "spdlog")

    -- threading support (POSIX)
    if is_plat("linux", "macosx") then
        add_syslinks("pthread")
    end
target_end()

-- test target
target("threadlet_tests")
    set_kind("binary")
    set_rundir("$(projectdir)/tests")

    -- link
    add_deps("threadlet")

    -- dependencies
    add_packages("stduuid", "spdlog", "doctest")

    -- sources
    add_files("tests/*.cpp")
target_end()