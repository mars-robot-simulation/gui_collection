project(log_console)
set(PROJECT_VERSION 1.0)
set(PROJECT_DESCRIPTION "The library provides a console module.")
cmake_minimum_required(VERSION 2.6)

include(FindPkgConfig)

find_package(lib_manager)
lib_defaults()
define_module_info()

# IF (APPLE)
#   EXEC_PROGRAM(uname ARGS -v  OUTPUT_VARIABLE DARWIN_VERSION)
#   STRING(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
#   MESSAGE(STATUS "DARWIN_VERSION=${DARWIN_VERSION}")
#   IF (DARWIN_VERSION EQUAL 11)
#     MESSAGE("use gnu compiler")
#     set(CMAKE_C_COMPILER "gcc-apple-4.2")
#     set(CMAKE_CXX_COMPILER "g++-apple-4.2")
#   ENDIF (DARWIN_VERSION EQUAL 11)
# ENDIF(APPLE)

setup_qt()

pkg_check_modules(PKGCONFIG REQUIRED
        lib_manager
        cfg_manager
        data_broker
        main_gui
)
include_directories(${PKGCONFIG_INCLUDE_DIRS})
link_directories(${PKGCONFIG_LIBRARY_DIRS})
add_definitions(${PKGCONFIG_CLFAGS_OTHER})  #flags excluding the ones with -I

link_directories(${QT_LIBRARY_DIR})

include_directories(src)


set(SOURCES
	src/ConsoleGUI.cpp
	src/MainConsole.cpp
)

set(HEADERS
	src/ConsoleGUI.h
	src/ConsoleInterface.h
	src/MainConsole.h
)

set(QT_MOC_HEADER
	src/ConsoleGUI.h
	src/MainConsole.h
)

add_definitions(-DMARS_DEFAULT_RESOURCES_PATH=\"${CMAKE_INSTALL_PREFIX}/share\")

if (${USE_QT5})
qt5_wrap_cpp ( QT_MOC_HEADER_SRC ${QT_MOC_HEADER} )
else (${USE_QT5})
qt4_wrap_cpp ( QT_MOC_HEADER_SRC ${QT_MOC_HEADER} )
endif (${USE_QT5})

add_library(${PROJECT_NAME} SHARED ${SOURCES} ${QT_MOC_HEADER_SRC} )

if (${USE_QT5})
qt5_use_modules(${PROJECT_NAME} Widgets)
endif (${USE_QT5})

target_link_libraries(${PROJECT_NAME}
                      ${PKGCONFIG_LIBRARIES}
                      ${QT_LIBRARIES}
)

if(WIN32)
  set(LIB_INSTALL_DIR bin) # .dll are in PATH, like executables
else(WIN32)
  set(LIB_INSTALL_DIR lib)
endif(WIN32)


set(_INSTALL_DESTINATIONS
	RUNTIME DESTINATION bin
	LIBRARY DESTINATION ${LIB_INSTALL_DIR}
	ARCHIVE DESTINATION lib
)


# Install the library into the lib folder
install(TARGETS ${PROJECT_NAME} ${_INSTALL_DESTINATIONS})

# Install headers into mars include directory
install(FILES ${HEADERS} DESTINATION include/${PROJECT_NAME})

# Prepare and install necessary files to support finding of the library
# using pkg-config
configure_file(${PROJECT_NAME}.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc DESTINATION lib/pkgconfig)

INSTALL(DIRECTORY resources DESTINATION share/${PROJECT_NAME})
