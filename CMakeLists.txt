cmake_minimum_required (VERSION 3.8)

project (Xertz)
find_package(ZLIB REQUIRED)
include_directories(zlib)
set(CMAKE_CXX_STANDARD 20)

#target_link_libraries(Xertz ${ZLIB_LIBRARIES})
include_directories(${ZLIB_INCLUDE_DIRS})

