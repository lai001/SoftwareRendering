set_xmakever("2.6.3")
add_requires("spdlog")
add_requires("assimp")
add_requires("glad")
add_requires("glfw")
add_requires("glm")
add_requires("stb")

rule("ProjectSetting")
    on_load(function (target)
        import("core.project.config")
        if is_plat("windows") then
            config.set("vs_sdkver", "10.0.17763.0")
        end
    end)
 
rule("CopyResource")
    after_build(function (target)
        os.cp("Resource", path.join(target:targetdir(), "Resource"))
    end)

target("SoftwareRendering")
    set_kind("binary")
    add_files("Src/**.cpp")
    add_headerfiles("Src/**.hpp")
    add_includedirs("Src")
    add_includedirs("Src/Shader")
    add_rules("mode.debug", "mode.release")
    add_rules("ProjectSetting")
    add_rules("CopyResource")
    add_packages("spdlog")
    add_packages("assimp")
    add_packages("glad")
    add_packages("glfw")
    add_packages("glm")
    add_packages("stb")
