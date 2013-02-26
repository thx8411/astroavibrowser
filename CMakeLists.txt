####################################
# CMakeLists for astroavibrowser   #
# (c) Blaise-Florentin Collin 2013 #
#       under GPL v3 License       #
####################################  

cmake_minimum_required(VERSION 2.6.8)

####################
# BUILD DIRECTIVES #
####################

# distribution detection
find_file (DEBIAN_FOUND debian_version debconf.conf PATHS /etc)
find_file (FEDORA_FOUND fedora-release PATHS /etc)

# project name
project(astroavibrowser)

# version
set(DEB_EPOCH 0)
set(DEB_REVISION 1)
set(RPM_REVISION 1)
set(MAJOR_VERSION "0")
set(MINOR_VERSION "4")
set(PATCH_VERSION "beta20130225")
set(BUILD_VERSION "33")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}${PATCH_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D BINARY_VERSION=\\\"${FULL_VERSION}\\\" -D BUILD_VERSION=${BUILD_VERSION}")

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
install(FILES doc/astroavibrowser_help.html DESTINATION /usr/share/astroavibrowser)
install(FILES doc/BUGS DESTINATION /usr/share/astroavibrowser)
install(FILES doc/TODO DESTINATION /usr/share/astroavibrowser)
install(FILES doc/Contributors DESTINATION /usr/share/astroavibrowser)
install(FILES doc/ChangeLog DESTINATION /usr/share/astroavibrowser)
install(FILES doc/license.txt DESTINATION /usr/share/astroavibrowser)
install(FILES doc/INSTALL DESTINATION /usr/share/astroavibrowser)
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
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Video editing tool for astronomy")
set(CPACK_PACKAGE_DESCRIPTION "astroavibrowser is a small tool for astronomy imaging capture/processing.")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION} With astroavibrowser, you may open a video file")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}, select the good frames and save the new sequence")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION} in a new avi file. It also debayers your raw sequences.")
set(CPACK_PACKAGE_VENDOR "Thx8411")
set(CPACK_PACKAGE_CONTACT "thx8411@users.sourceforge.net")
set(CPACK_STRIP_FILES astroavibrowser)

# debian package
if(DEBIAN_FOUND)
   set(CPACK_GENERATOR DEB)
   set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCHITECTURE})
   set(CPACK_DEBIAN_PACKAGE_NAME "astroavibrowser")
   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libavcodec53 (>= 4:0.8-1~) | libavcodec-extra-53 (>= 4:0.8-1~)
         , libavformat53 (>= 4:0.8-1~) | libavformat-extra-53 (>= 4:0.8-1~)
         , libavutil51 (>= 4:0.8-1~) | libavutil-extra-51 (>= 4:0.8-1~)
         , libc6 (>= 2.14), libgcc1 (>= 1:4.1.1), libqtcore4 (>= 4:4.5.3)
         , libqtgui4 (>= 4:4.5.3), libstdc++6 (>= 4.1.1)
         , libswscale2 (>= 4:0.8-1~) | libswscale-extra-2 (>= 4:0.8-1~)")
   set(CPACK_DEBIAN_PACKAGE_SECTION graphics)
   set(CPACK_DEBIAN_PACKAGE_VERSION ${DEB_EPOCH}:${FULL_VERSION}-${DEB_REVISION})
   set(CPACK_PACKAGE_FILE_NAME ${CPACK_DEBIAN_PACKAGE_NAME}_${FULL_VERSION}-${DEB_REVISION}_${ARCHITECTURE})
   set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${CPACK_PACKAGE_DESCRIPTION}")
endif(DEBIAN_FOUND)

# rpm package
if(FEDORA_FOUND)
   set(CPACK_GENERATOR RPM)
   #
   # to do...
   #
endif(FEDORA_FOUND)

include(CPack)
