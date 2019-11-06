add_library(png STATIC "")
target_include_directories(png PUBLIC "")
target_link_libraries(png PUBLIC zlib Base)
#target_link_libraries(png PUBLIC zlib)
target_compile_definitions(png PRIVATE _CRT_SECURE_NO_WARNINGS)
if(${KIGS_PLATFORM} STREQUAL "Android")
	target_compile_definitions(png PRIVATE ANDROID)
endif()

target_sources(png 
	PRIVATE
		"png.c"
		"pngerror.c"
		"pngget.c"
		"pngmem.c"
		"pngpread.c"
		"pngread.c"
		"pngrio.c"
		"pngrtran.c"
		"pngrutil.c"
		"pngset.c"
		"pngtrans.c"
		"pngwio.c"
		"pngwrite.c"
		"pngwtran.c"
		"pngwutil.c")






