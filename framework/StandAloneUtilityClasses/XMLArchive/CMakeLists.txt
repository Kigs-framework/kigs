add_library(XMLArchive INTERFACE)

target_include_directories(XMLArchive INTERFACE "Headers")

file(GLOB_RECURSE allcpps "Sources/*")
target_sources(XMLArchive INTERFACE ${allcpps})
