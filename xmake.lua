add_rules("mode.debug", "mode.release")

option("build_reflection_test")
    set_showmenu(true)
    set_default(true)
    set_values(true, false)
    set_languages("c++20")
    add_includedirs("$(projectdir)/src")
option_end()

if has_config("build_reflection_test") then
    includes("test")
end