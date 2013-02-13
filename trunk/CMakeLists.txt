cmake_minimum_required(VERSION 2.6.8)

# project name
project(AstroAviBrowser)

# version
set(MAJOR_VERSION "0")
set(MINOR_VERSION "4")
set(PATCH_VERSION "beta20130213")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}${PATCH_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D BINARY_VERSION=\\\"${FULL_VERSION}\\\"")

# destir for binary packages
set($ENV{DESTDIR} "${CMAKE_BINARY_DIR}/fakeinstall")

# add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# needed packages
find_package(Qt4 REQUIRED)
find_package(LibAV REQUIRED)

# source files
set(AstroAviBrowser_SOURCES
	src/main.cpp
	src/aviwriter.cpp
	src/Qframelist.cpp
	src/bmpwriter.cpp
	src/processing.cpp
	src/Qhistogram.cpp
	src/filewriter.cpp
	src/Qframedisplay.cpp
	src/Qmainwindow.cpp)

# we use Qt4
set(CMAKE_AUTOMOC TRUE)
include(${QT_USE_FILE})
add_definitions(${QT_DEFINITIONS})

# we use libav
include_directories(${LIBAV_INCLUDE_DIRS})
add_definitions(${LIBAV_DEFINITIONS})

# add raw types for libav
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D __STDC_CONSTANT_MACROS")

# target
add_executable(AstroAviBrowser ${AstroAviBrowser_SOURCES})

# target libs
target_link_libraries(AstroAviBrowser ${QT_LIBRARIES} ${LIBAV_LIBRARIES})

# installation
install(TARGETS AstroAviBrowser RUNTIME DESTINATION /usr/bin)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/icons)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/astroavibrowser/icons)
install(FILES astroavibrowser-entry.desktop DESTINATION /usr/share/applications)

# package generation

set(CPACK_SET_DESTDIR "ON")
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ".svn")

set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "astroavibrowser_${FULL_VERSION}_src" CACHE INTERNAL "tarball basename")

include(CPack)


