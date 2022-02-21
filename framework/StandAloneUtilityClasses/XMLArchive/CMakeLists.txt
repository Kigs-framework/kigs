
file(GLOB_RECURSE all_headers "Headers/*.h")
add_library(XMLArchive STATIC  
    ${all_headers}
    "Sources/XLSXCellRowCol.cpp"
    "Sources/XLSXDocument.cpp"
    "Sources/XLSXElementRef.cpp"
    "Sources/XLSXSheet.cpp"
    "Sources/XMLArchiveManager.cpp")

target_include_directories(XMLArchive PUBLIC "Headers")
target_link_libraries(XMLArchive PUBLIC Core XML miniz)