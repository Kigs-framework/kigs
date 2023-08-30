

add_library(eigen INTERFACE "")

target_include_directories(eigen INTERFACE "../")
target_compile_definitions(eigen INTERFACE _SILENCE_CXX23_DENORM_DEPRECATION_WARNING)

