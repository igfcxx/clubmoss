-- =============================================================================
--  项目
-- =============================================================================

set_project("clubmoss")
set_version("0.1.0")

add_rules("mode.release", "mode.debug")
set_allowedmodes("release", "debug")
set_allowedarchs("x86_64", "x64")
set_languages("c99", "cxx26")
set_warnings("all", "error")

add_cxxflags("-Wconversion", "-Wno-dangling-reference")

if is_mode("debug") then
    add_defines("DEBUG")
    if is_plat("mingw") then
        add_ldflags("-Wl,--allow-multiple-definition")
    end
elseif is_mode("release") then
    set_fpmodels("fast", "noexcept")
end

-- =============================================================================
--  依赖
-- =============================================================================

add_requires("openmp")
add_requires("spdlog    >=1.17", {debug = true})
add_requires("toml11    >=4.4",  {debug = true})
add_requires("doctest   >=2.5",  {debug = true})
add_requires("nanobench >=4.3",  {debug = true})

-- =============================================================================
--  构建
-- =============================================================================

add_defines("TOML11_NO_ERROR_PREFIX")

target("core")
    set_kind("static")
    add_files("src/core/**.cpp")
    add_packages("openmp", "toml11")

add_defines("DOCTEST_CONFIG_USE_STD_HEADERS")

target("unit_tests_4_core")
    set_kind("binary")
    add_includedirs("tests")
    add_packages("openmp", "toml11", "doctest")
