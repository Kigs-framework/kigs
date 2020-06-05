project(TextureCompress)

add_subdirectory(EtcLib)

add_executable(TextureCompress "")

include_directories(./EtcLib/Etc)
include_directories(./EtcLib/EtcCodec)

set(TextureCompress_GUID_CMAKE "1C3AEB3D-6195-4F7F-8159-F74AE83702DF" CACHE INTERNAL "TextureCompress GUID")

target_link_libraries(TextureCompress PRIVATE BaseApplication TinyImage zlib EtcLib zstdlib)


target_include_directories(TextureCompress PRIVATE "Headers" "squish/Headers" "DMP" "zlib")

file(GLOB_RECURSE all_sources "Sources/*.cpp")
file(GLOB_RECURSE all_headers "Headers/*.h")
file(GLOB_RECURSE zip_sources "zlib/*.*")
file(GLOB_RECURSE squish_headers "squish/Headers/*.h")
file(GLOB_RECURSE squish_sources "squish/Sources/*.cpp")

target_sources(TextureCompress 
	PRIVATE 
		${all_sources}
		${all_headers}
		${zip_sources}
		${squish_headers}
		${squish_sources}
		)



