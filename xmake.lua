add_rules("mode.debug", "mode.release")

add_requires("glfw", "vulkansdk", "cglm")
add_requires("glslang", { configs = { binaryonly = true } })

target("lovelace")
set_kind("binary")
add_rules("utils.glsl2spv", { bin2c = true })
add_files("src/**.c")
add_files("glsl/**.frag")
add_files("glsl/**.vert")
add_packages("glfw", "vulkansdk", "cglm")
add_packages("glslang")
