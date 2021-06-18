add_library(CoreFSM INTERFACE)

target_include_directories(CoreFSM INTERFACE "Headers")

file(GLOB_RECURSE allcpps "Sources/*")
target_sources(CoreFSM INTERFACE ${allcpps})
