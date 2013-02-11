cmake_minimum_required(VERSION 2.6.8)

# project name
project(AstroAviBrowser)

# add cmake module path
set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/cmake/Modules/")

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
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

# target
add_executable(AstroAviBrowser ${AstroAviBrowser_SOURCES})

# target libs
target_link_libraries(AstroAviBrowser ${QT_LIBRARIES} ${LIBAV_LIBRARIES})

# installation
install(TARGETS AstroAviBrowser RUNTIME DESTINATION /usr/bin)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/icons)
install(FILES icons/astroavibrowser-icon.png DESTINATION /usr/share/astroavibrowser/icons)
install(FILES astroavibrowser-entry.desktop DESTINATION /usr/share/applications)
