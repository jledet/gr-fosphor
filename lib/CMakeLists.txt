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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX
include(GrMiscUtils)

add_custom_command(
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/fosphor
  OUTPUT fosphor/resource_data.c
  DEPENDS fosphor/fft.cl fosphor/display.cl fosphor/cmap_simple.glsl fosphor/cmap_bicubic.glsl fosphor/cmap_fallback.glsl fosphor/DroidSansMonoDotted.ttf
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fosphor/llist.h ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/fosphor/resource_internal.h ${CMAKE_CURRENT_BINARY_DIR}/fosphor/
  COMMAND ${PYTHON_EXECUTABLE} -B mkresources.py fft.cl display.cl cmap_simple.glsl cmap_bicubic.glsl cmap_fallback.glsl DroidSansMonoDotted.ttf > ${CMAKE_CURRENT_BINARY_DIR}/fosphor/resource_data.c
)

if(WIN32)
   find_package(GLEW REQUIRED)
   include_directories(${GLEW_INCLUDE_DIRS})
   list(APPEND fosphor_libraries ${GLEW_LIBRARIES})
   add_definitions(-DENABLE_GLEW)
endif(WIN32)

if(ENABLE_PYTHON)
    add_definitions(-DENABLE_PYTHON)
    include_directories(${PYTHON_INCLUDE_DIRS})
endif(ENABLE_PYTHON)

if(ENABLE_GLFW)
    include_directories(${GLFW3_INCLUDE_DIRS})
    link_directories(${GLFW3_LIBRARY_DIRS})
endif(ENABLE_GLFW)

if(ENABLE_QT)
    set(CMAKE_AUTOMOC ON)
    include_directories(${Qt5_INCLUDE_DIRS})
endif(ENABLE_QT)

list(APPEND fosphor_sources
	fosphor/axis.c
	fosphor/cl.c
	fosphor/cl_compat.c
	fosphor/fosphor.c
	fosphor/gl.c
	fosphor/gl_cmap.c
	fosphor/gl_cmap_gen.c
	fosphor/gl_font.c
	fosphor/resource.c
	fosphor/resource_data.c
	fifo.cc
	base_sink_c_impl.cc
)

list_cond_append(ENABLE_GLFW fosphor_sources glfw_sink_c_impl.cc)
list_cond_append(ENABLE_QT   fosphor_sources QGLSurface.cc qt_sink_c_impl.cc)

list(APPEND fosphor_libraries 
	${OPENGL_LIBRARIES}
	${OpenCL_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${Boost_LIBRARIES}
	${GNURADIO_ALL_LIBRARIES}
	${CMAKE_DL_LIBS}
)

list_cond_append(ENABLE_PYTHON fosphor_libraries ${PYTHON_LIBRARY})
list_cond_append(ENABLE_GLFW fosphor_libraries ${GLFW3_LIBRARIES})
list_cond_append(ENABLE_QT   fosphor_libraries ${Qt5_LIBRARIES})

add_library(gnuradio-fosphor SHARED ${fosphor_sources})
target_link_libraries(gnuradio-fosphor gnuradio::gnuradio-runtime gnuradio::gnuradio-fft)
target_include_directories(gnuradio-fosphor
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    PUBLIC $<INSTALL_INTERFACE:include>
  )
target_include_directories(gnuradio-fosphor PUBLIC
	${OPENGL_INCLUDE_DIRS}
	${OpenCL_INCLUDE_DIRS}
	${FREETYPE_INCLUDE_DIRS})
target_link_directories(gnuradio-fosphor PUBLIC
	${OPENGL_LIBRARY_DIRS}
	${OpenCL_LIBRARY_DIRS}
	${FREETYPE_LIBRARY_DIRS})
set_target_properties(gnuradio-fosphor PROPERTIES DEFINE_SYMBOL "gnuradio_fosphor_EXPORTS")
target_link_libraries(gnuradio-fosphor ${fosphor_libraries})
target_compile_definitions(gnuradio-fosphor PRIVATE QT_NO_KEYWORDS)


GR_LIBRARY_FOO(gnuradio-fosphor)
