# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of gr-fosphor
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.8)
project(gr-fosphor CXX C)
enable_testing()

# Install to PyBOMBS target prefix if defined
if(DEFINED ENV{PYBOMBS_PREFIX})
    set(CMAKE_INSTALL_PREFIX $ENV{PYBOMBS_PREFIX})
    message(STATUS "PyBOMBS installed GNU Radio. Setting CMAKE_INSTALL_PREFIX to $ENV{PYBOMBS_PREFIX}")
endif()

# Select the release build type by default to get optimization flags
if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE "Release")
   message(STATUS "Build type not specified: defaulting to release.")
endif(NOT CMAKE_BUILD_TYPE)
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "")

# Make sure our local CMake Modules path comes first
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

# Set the version information here
set(VERSION_INFO_MAJOR_VERSION 3)
set(VERSION_INFO_API_COMPAT    8)
set(VERSION_INFO_MINOR_VERSION 0)
set(VERSION_INFO_MAINT_VERSION git)

cmake_policy(SET CMP0011 NEW)

# Enable generation of compile_commands.json for code completion engines
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

########################################################################
# Compiler specific setup
########################################################################
if((CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    AND NOT WIN32)
    #http://gcc.gnu.org/wiki/Visibility
    add_definitions(-fvisibility=hidden)
endif()

IF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_CXX_STANDARD 11)
ELSEIF(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_STANDARD 11)
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_CXX_STANDARD 11)
ELSE()
    message(WARNING "C++ standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

IF(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    SET(CMAKE_C_STANDARD 11)
ELSEIF(CMAKE_C_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_C_STANDARD 11)
ELSEIF(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    SET(CMAKE_C_STANDARD 11)
ELSE()
    message(WARNING "C standard could not be set because compiler is not GNU, Clang or MSVC.")
ENDIF()

########################################################################
# Install directories
########################################################################
find_package(Gnuradio "3.8" REQUIRED fft)
include(GrVersion)

include(GrPlatform) #define LIB_SUFFIX

if(NOT CMAKE_MODULES_DIR)
  set(CMAKE_MODULES_DIR lib${LIB_SUFFIX}/cmake)
endif(NOT CMAKE_MODULES_DIR)

set(GR_INCLUDE_DIR      include/fosphor)
set(GR_CMAKE_DIR        ${CMAKE_MODULES_DIR}/fosphor)
set(GR_PKG_DATA_DIR     ${GR_DATA_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_DOC_DIR      ${GR_DOC_DIR}/${CMAKE_PROJECT_NAME})
set(GR_PKG_CONF_DIR     ${GR_CONF_DIR}/${CMAKE_PROJECT_NAME}/conf.d)
set(GR_PKG_LIBEXEC_DIR  ${GR_LIBEXEC_DIR}/${CMAKE_PROJECT_NAME})

########################################################################
# On Apple only, set install name and use rpath correctly, if not already set
########################################################################
if(APPLE)
    if(NOT CMAKE_INSTALL_NAME_DIR)
        set(CMAKE_INSTALL_NAME_DIR
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install Name Destination Directory" FORCE)
    endif(NOT CMAKE_INSTALL_NAME_DIR)
    if(NOT CMAKE_INSTALL_RPATH)
        set(CMAKE_INSTALL_RPATH
            ${CMAKE_INSTALL_PREFIX}/${GR_LIBRARY_DIR} CACHE
            PATH "Library Install RPath" FORCE)
    endif(NOT CMAKE_INSTALL_RPATH)
    if(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
        set(CMAKE_BUILD_WITH_INSTALL_RPATH ON CACHE
            BOOL "Do Build Using Library Install RPath" FORCE)
    endif(NOT CMAKE_BUILD_WITH_INSTALL_RPATH)
endif(APPLE)

########################################################################
# Find gr-fosphor build dependencies
########################################################################

# Required
set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL)
if(NOT OPENGL_FOUND)
	message(FATAL_ERROR "OpenGL required to compile gr-fosphor")
endif()

find_package(OpenCL)
if(NOT OpenCL_FOUND)
	message(FATAL_ERROR "OpenCL required to compile gr-fosphor")
endif()

find_package(Freetype)
if(NOT FREETYPE_FOUND)
	message(FATAL_ERROR "freetype2 required to compile gr-fosphor")
endif()

# Optional
find_package(GLFW3)

set(Qt5_REQUIRED_COMPONENTS Core Gui OpenGL)
find_package(Qt5 5.7.0 COMPONENTS ${Qt5_REQUIRED_COMPONENTS})
if (Qt5_FOUND)
	foreach(module ${Qt5_REQUIRED_COMPONENTS})
		list(APPEND Qt5_INCLUDE_DIRS ${Qt5${module}_INCLUDE_DIRS})
		list(APPEND Qt5_LIBRARIES ${Qt5${module}_LIBRARIES})
	endforeach(module)
endif (Qt5_FOUND)

########################################################################
# Find gnuradio build dependencies
########################################################################
find_package(Doxygen)

########################################################################
# Setup doxygen option
########################################################################
if(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" ON)
else(DOXYGEN_FOUND)
    option(ENABLE_DOXYGEN "Build docs using Doxygen" OFF)
endif(DOXYGEN_FOUND)

########################################################################
# Setup the components
########################################################################
include(GrComponent)

GR_REGISTER_COMPONENT("GLFW" ENABLE_GLFW
    GLFW3_FOUND
)

GR_REGISTER_COMPONENT("QT" ENABLE_QT
    Qt5_FOUND
)

macro(list_cond_append cond list_name)
    if(${cond})
        list(APPEND ${list_name} ${ARGN})
    endif(${cond})
endmacro(list_cond_append)

########################################################################
# Create uninstall target
########################################################################
configure_file(
    ${CMAKE_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
@ONLY)

add_custom_target(uninstall
    ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include/gnuradio/fosphor)
add_subdirectory(lib)

if(ENABLE_PYTHON)
    add_subdirectory(swig)
    add_subdirectory(python)
    add_subdirectory(grc)
    add_subdirectory(apps)
endif(ENABLE_PYTHON)

add_subdirectory(docs)

########################################################################
# Create Pkg Config File
########################################################################
file(TO_NATIVE_PATH "${CMAKE_INSTALL_PREFIX}"           prefix)
file(TO_NATIVE_PATH "\${prefix}"                        exec_prefix)
file(TO_NATIVE_PATH "\${exec_prefix}/${GR_LIBRARY_DIR}" libdir)
file(TO_NATIVE_PATH "\${prefix}/${GR_INCLUDE_DIR}"      includedir)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fosphor.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fosphor.pc
@ONLY)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fosphor.pc
	DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

########################################################################
# Print Summary
########################################################################
GR_PRINT_COMPONENT_SUMMARY()
MESSAGE(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
