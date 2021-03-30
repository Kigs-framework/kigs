add_library(miniz STATIC "")
target_include_directories(miniz PUBLIC ".")
target_sources(miniz PRIVATE "miniz.c")