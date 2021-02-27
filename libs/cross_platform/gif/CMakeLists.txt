add_library(gif STATIC "")
target_include_directories(gif PUBLIC "")
target_compile_definitions(gif PRIVATE _CRT_SECURE_NO_WARNINGS)
target_sources(gif 
	PRIVATE 
		"dgif_lib.c"
		"egif_lib.c"
		"gif_err.c"
		"gif_font.c"
		"gif_hash.c"
		"gifalloc.c"
		"openbsd-reallocarray.c"
		"quantize.c")

