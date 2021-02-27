add_library(GPSCourse INTERFACE)

target_include_directories(GPSCourse INTERFACE "Headers")

file(GLOB_RECURSE allcpps "Sources/*")
target_sources(GPSCourse INTERFACE ${allcpps})
