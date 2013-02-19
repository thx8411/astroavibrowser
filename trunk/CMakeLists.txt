cmake_minimum_required(VERSION 2.6.8)

####################
# BUILD DIRECTIVES #
####################

# project name
project(astroavibrowser)

# version
set(MAJOR_VERSION "0")
set(MINOR_VERSION "4")
set(PATCH_VERSION "beta20130219")
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
set(astroavibrowser_SOURCES
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
add_executable(astroavibrowser ${astroavibrowser_SOURCES})

# target libs
target_link_libraries(astroavibrowser ${QT_LIBRARIES} ${LIBAV_LIBRARIES})

# installation
install(TARGETS astroavibrowser RUNTIME DESTINATION /usr/bin)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/icons)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/astroavibrowser/icons)
install(FILES astroavibrowser-entry.desktop DESTINATION /usr/share/applications)

######################
# PACKAGE GENERATION #
######################

# target architecture detection
set(ARCHITECTURE "unknow")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
   set(ARCHITECTURE "amd64")
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")
   set(ARCHITECTURE "i386")
endif(CMAKE_SYSTEM_PROCESSOR STREQUAL "i386")

# global directives
set(CPACK_SET_DESTDIR "ON")
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})

# source package
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ".svn")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "astroavibrowser_${FULL_VERSION}_src" CACHE INTERNAL "tarball basename")

# global directives for debian and rpm packages
set(CPACK_PACKAGE_DESCRIPTION "Video editing tool for astronomy")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "astroavibrowser is a small tool for astronomy imaging capture/processing. With astroavibrowser, you may open a video file, select the good frames and save the new sequence in a new avi file. It also debayers your raw sequences.")
set(CPACK_PACKAGE_VENDOR "Thx8411")
set(CPACK_PACKAGE_CONTACT "thx8411@users.sourceforge.net")

# debian package
set(CPACK_GENERATOR DEB)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCHITECTURE})
set(CPACK_DEBIAN_PACKAGE_NAME "astroavibrowser_${FULL_VERSION}_${ARCHITECTURE}.deb")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libavcodec52 (>= 3:0.svn20090303-1) | libavcodec-unstripped-52 (>= 3:0.svn20090303-1), libavformat52 (>= 3:0.svn20090303-1) | libavformat-unstripped-52 (>= 3:0.svn20090303-1), libavifile-0.7c2 (>= 1:0.7.47.20070718), libavutil49 (>= 3:0.svn20090303-1) | libavutil-unstripped-49 (>= 3:0.svn20090303-1), libc6 (>= 2.4), libgcc1 (>= 1:4.1.1), libqtcore4 (>= 4.5.0~+rc1), libqtgui4 (>= 4.5.0~+rc1), libstdc++6 (>= 4.1.1), libswscale0 (>= 3:0.svn20090303-1) | libswscale-unstripped-0 (>= 3:0.svn20090303-1)")
set(CPACK_DEBIAN_PACKAGE_SECTION graphics)

# rpm package
#set(CPACK_GENERATOR RPM)
#
# to do...
#

include(CPack)
