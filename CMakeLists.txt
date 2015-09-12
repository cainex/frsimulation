#****************************************************************************
# Copyright (c) 2015 by Daniel Grabowski.
#
# This file is part of FrSim.
# 
# FrSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# FrSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with FrSim.  If not, see <http://www.gnu.org/licenses/>.
#
# Created by: Daniel Grabowski, Andrew Worcester
#
#****************************************************************************

cmake_minimum_required(VERSION 2.8.11)
project (frverification)

set(FRVERIFICATION_MAJOR_VERSION 0)
set(FRVERIFICATION_MINOR_VERSION 1)
set(FRVERIFICATION_PATCH_VERSION 0)
set(FRVERIFICATION_VERSION
  ${FRVERIFICATION_MAJOR_VERSION}, ${FRVERIFICATION_MINOR_VERSION}, ${FRVERIFICATION_PATCH_VERSION})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/CMake/frverification)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation director for CMake files")

# Make relative paths absolute
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if (NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find foo/foo.h
  "${PROJECT_BINARY_DIR}")  # to find foo/config.h

enable_testing()

add_subdirectory(libfrsim)
add_subdirectory(libfrvm)

#####################################################

# Add all targets to the build-tree export set
export(TARGETS frsim frvm
  FILE "${PROJECT_BINARY_DIR}/frverificationTagets.cmake")

# Export the package for use from the build-tree
export(PACKAGE frverification)

# Create the frverificationConfig.cmake and frverificationConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
  "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(frverificationConfig.cmake.in
  "${PROJECT_BINARY_DIR}/frverificationConfig.cmake" @ONLY)
set(CONF_INCLUDE_DIRS "\${FRVERIFICATION_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(frverificationConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/frverificationConfig.cmake" @ONLY)
configure_file(frverificationConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/frverificationConfigVersion.cmake" @ONLY)

#install the frverificationConfig.cmake and frverificationConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/frverificationConfig.cmake"
  "${PROJECT_BINARY_DIR}/frverificationConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)


#install the export set for use with the install-tree
install(EXPORT frverificationTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)

