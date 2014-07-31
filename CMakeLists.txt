#Cmake version
cmake_minimum_required(VERSION 2.8)
project(Cells)

if(NOT CMAKE_BUILD_TYPE  AND NOT CMAKE_CONFIGURATION_TYPES)
    message( STATUS "Setting build type to 'Release as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
    #set possible values of built type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Release" "Debug"
        "MinSizeRel" "RelWithDebInfo")
endif()

set(EXECUTABLE_NAME "Cells")
set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
configure_file(
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )

#include sources, headers and runtime dependencies
set(HEADERS
  src/cells/board.h
  src/cells/cells.h
  src/cells/rulegrid.h
  src/cells/ruleset.h
  src/cells/selectionbox.h
  src/cells/settingsgui.h
  src/cells/tool.h
  src/configfile/configfile.h
  src/configfile/configoption.h
  src/configfile/strlib.h
  src/gui/button.h
  src/gui/buttonmap.h
  src/gui/cursor.h
  src/gui/groupbox.h
  src/gui/inputbox.h
  src/other/colorcode.h
  src/other/filenamegenerator.h
  src/other/matrix.h
)

set(SOURCES
  src/cells/board.cpp
  src/cells/cells.cpp
  src/cells/rulegrid.cpp
  src/cells/ruleset.cpp
  src/cells/selectionbox.cpp
  src/cells/settingsgui.cpp
  src/cells/tool.cpp
  src/cells/main.cpp
  src/configfile/configfile.cpp
  src/configfile/configoption.cpp
  src/configfile/strlib.cpp
  src/gui/button.cpp
  src/gui/buttonmap.cpp
  src/gui/cursor.cpp
  src/gui/groupbox.cpp
  src/gui/inputbox.cpp
  src/other/colorcode.cpp
  src/other/filenamegenerator.cpp
)

set(RUNTIME_DEPENDENCIES

)

#include directories
include_directories("${PROJECT_BINARY_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/src/cells")
include_directories("${PROJECT_SOURCE_DIR}/src/configfile")
include_directories("${PROJECT_SOURCE_DIR}/src/gui")
include_directories("${PROJECT_SOURCE_DIR}/src/other")

add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS} ${RUNTIME_DEPENDENCIES})

#copy any runtime dependencies into binary folder
add_custom_target(copy_runtime_dependencies ALL)
foreach(RUNTIME_DEPENDENCY ${RUNTIME_DEPENDENCIES})
  add_custom_command(TARGET copy_runtime_dependencies POST_BUILD
                     COMMAND ${CMAKE_COMMAND} -E copy
                     ${CMAKE_SOURCE_DIR}/${RUNTIME_DEPENDENCY}
                     $<TARGET_FILE_DIR:${EXECUTABLE_NAME}>/${RUNTIME_DEPENDENCY})
endforeach()

#set cmake module path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

#SFML libraries
find_package(SFML 2 REQUIRED system window graphics)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${EXECUTABLE_NAME} ${SFML_LIBRARIES})

list(APPEND CMAKE_CXX_FLAGS "-std=c++0x")
