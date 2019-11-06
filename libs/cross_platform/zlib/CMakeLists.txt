add_library(zlib STATIC "")
target_include_directories(zlib PUBLIC "")
target_compile_definitions(zlib PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(zlib 
	PRIVATE 
		"adler32.c"
		"compress.c"
		"crc32.c"
		"deflate.c"
		"infback.c"
		"inffast.c"
		"inflate.c"
		"inftrees.c"
		"gzlib.c"
		"gzread.c"
		"gzclose.c"
		"gzwrite.c"
		"trees.c"
		"uncompr.c"
		"zutil.c")

