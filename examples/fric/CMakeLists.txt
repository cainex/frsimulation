#****************************************************************************
# Copyright (c) 2015 by Daniel Grabowski.
#
# This file is part of FrIP.
# 
# FrIP is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# FrIP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with FrIP.  If not, see <http://www.gnu.org/licenses/>.
#
# Created by: Daniel Grabowski, Andrew Worcester
#
#****************************************************************************

cmake_minimum_required(VERSION 2.8.11)
project (fric)

set(FRIC_MAJOR_VERSION 0)
set(FRIC_MINOR_VERSION 1)
set(FRIC_PATCH_VERSION 0)
set(FRIC_VERSION
  ${FRIC_MAJOR_VERSION}, ${FRIC_MINOR_VERSION}, ${FRIC_PATCH_VERSION})

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for binaries")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_CMAKE_DIR lib/CMake/fric)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation director for CMake files")

add_subdirectory(sim)
add_subdirectory(rtl)

#########################################

