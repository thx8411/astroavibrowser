####################################
# CMakeLists for astroavibrowser   #
# (c) Blaise-Florentin Collin 2013 #
#       under GPL v3 License       #
####################################  

cmake_minimum_required(VERSION 2.8.6)


####################
# BUILD DIRECTIVES #
####################

# project name
project(astroavibrowser)

# distribution detection
find_file(DEBIAN_FOUND debian_version debconf.conf PATHS /etc)
find_file(FEDORA_FOUND fedora-release PATHS /etc)
find_file(MAGEIA_FOUND mageia-release PATHS /etc)

# version
set(DEB_EPOCH 0)
set(MAJOR_VERSION "0")
set(MINOR_VERSION "4")
set(PATCH_VERSION "4")
set(BUILD_VERSION "44")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D BINARY_VERSION=\\\"${FULL_VERSION}\\\" -D BUILD_VERSION=${BUILD_VERSION}")

# destir for binary packages
set($ENV{DESTDIR} "${CMAKE_BINARY_DIR}/fakeinstall")

# add cmake module path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# needed packages
find_package(Qt4 REQUIRED)
if(NOT QT_FOUND)
   message(FATAL_ERROR "Qt4 not found, generation stopped")
endif(NOT QT_FOUND)

find_package(LibAV REQUIRED)
if(NOT LIBAV_FOUND)
   message(FATAL_ERROR "LibAv not found, generation stopped")
endif(NOT LIBAV_FOUND)

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

# default arch
set(ARCHITECTURE "unkown")

# global directives
set(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})


##################
# source package #
##################

# source package
set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_IGNORE_FILES ".svn")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "astroavibrowser_${FULL_VERSION}_src" CACHE INTERNAL "tarball basename")


#####################
# binaries packages #
#####################

# global directives for debian and rpm packages
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Video editing tool for astronomy")
set(CPACK_PACKAGE_DESCRIPTION "astroavibrowser is a small tool for astronomy imaging capture/processing.")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION} With astroavibrowser, you may open a video file")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION}, select the good frames and save the new sequence")
set(CPACK_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION} in a new avi file. It also debayers your raw sequences.")
set(CPACK_PACKAGE_VENDOR "Thx8411")
set(CPACK_PACKAGE_CONTACT "thx8411@users.sourceforge.net")
set(CPACK_STRIP_FILES astroavibrowser)


##################
# debian package #
##################

if(DEBIAN_FOUND)

   # arch detection
   find_program(DPKG_PROGRAM dpkg DOC "dpkg program of Debian-based systems")
   if(DPKG_PROGRAM)
     execute_process(
        COMMAND ${DPKG_PROGRAM} --print-architecture
        OUTPUT_VARIABLE ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
     )
   endif(DPKG_PROGRAM)

   # settings
   set(CPACK_GENERATOR DEB)
   set(CPACK_SET_DESTDIR "ON")
   set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${ARCHITECTURE})
   set(CPACK_DEBIAN_PACKAGE_NAME "astroavibrowser")

   # dependencies
   set(CPACK_DEBIAN_PACKAGE_DEPENDS "libavcodec53 | libavcodec-extra-53
         , libavformat53 | libavformat-extra-53
         , libavutil51 | libavutil-extra-51
         , libc6, libgcc1, libqtcore4
         , libqtgui4, libstdc++6
         , libswscale2 | libswscale-extra-2")
   set(CPACK_DEBIAN_PACKAGE_SECTION graphics)
   set(CPACK_DEBIAN_PACKAGE_VERSION "${DEB_EPOCH}:${FULL_VERSION}-1")
   set(CPACK_PACKAGE_FILE_NAME ${CPACK_DEBIAN_PACKAGE_NAME}_${FULL_VERSION}_${ARCHITECTURE})
   set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n ${CPACK_PACKAGE_DESCRIPTION}")

endif(DEBIAN_FOUND)


###############
# rpm package #
###############

if(FEDORA_FOUND OR MAGEIA_FOUND)

   # arch detection
   execute_process(COMMAND  uname -m
      OUTPUT_VARIABLE ARCHITECTURE
      OUTPUT_STRIP_TRAILING_WHITESPACE
   )

   # settings
   set(CPACK_GENERATOR RPM)
   set(CPACK_SET_DESTDIR "OFF")
   set(CPACK_RPM_PACKAGE_ARCHITECTURE ${ARCHITECTURE})
   set(CPACK_RPM_PACKAGE_LICENSE "GPL v3")
   set(CPACK_RPM_PACKAGE_GROUP "Productivity/Scientific/Astronomy")
   set(CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})
   set(CPACK_RPM_PACKAGE_NAME "astroavibrowser")
   set(CPACK_PACKAGE_FILE_NAME ${CPACK_RPM_PACKAGE_NAME}_${FULL_VERSION}.${ARCHITECTURE})
   set(CPACK_RPM_PACKAGE_RELOCATABLE FALSE)

   # dependencies
   if(ARCHITECURE STREQUAL "x86_64")
      set(CPACK_RPM_PACKAGE_REQUIRES "lib64avformat53,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} lib64avcodec53,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} lib64avutil51,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} lib64swscaler2,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} lib64qtcore4,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} lib64qtgui4")
   else(ARCHITECURE STREQUAL "x86_64")
      set(CPACK_RPM_PACKAGE_REQUIRES "libavformat53,") 
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} libavcodec53,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} libavutil51,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} libswscaler2,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} libqtcore4,")
      set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES} libqtgui4")
   endif(ARCHITECURE STREQUAL "x86_64")

endif(FEDORA_FOUND OR MAGEIA_FOUND)

include(CPack)
