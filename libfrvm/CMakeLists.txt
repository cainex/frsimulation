#****************************************************************************
# Copyright (c) 2015 by Daniel Grabowski.
#
# This file is part of FrVm.
# 
# FrVm is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# FrVm is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with FrVm.  If not, see <http://www.gnu.org/licenses/>.
#****************************************************************************


if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(warnings "-Wall -Wextra -Werror")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}  ${CMAKE_CURRENT_SOURCE_DIR}/../libfrsim/include)

set (SOURCE_FILES 
  FrVmConstraint.cpp  
  FrVmConstraintRange.cpp  
  FrVmEnv.cpp  
  FrVmVoter.cpp  
  FrVmXactor.cpp)

set (HEADER_FILES 
  FrVmCallback.h
  FrVmChannel.h
  FrVmConsensus.h
  FrVmConstraint.h
  FrVmConstraintRange.h
  FrVmData.h
  FrVmEnv.h
  FrVmVoter.h
  FrVmXactor.h  
  )

#set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
#add_library(frvm STATIC ${SOURCE_FILES})
add_library(frvm SHARED ${SOURCE_FILES})

target_include_directories(frvm PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>  
    $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}/frvm>  # <prefix>/include/mylib
)

set_target_properties(frvm PROPERTIES
  PUBLIC_HEADER "${HEADER_FILES}")

#install(TARGETS frvm
#  EXPORT frverificationTargets
#  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT dev
#  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/frvm" COMPONENT dev)
install(TARGETS frvm
  EXPORT frverificationTargets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/frvm" COMPONENT dev)



