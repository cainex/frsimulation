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
#****************************************************************************

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(warnings "-Wall -Wextra -Werror")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set (SOURCE_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimCond.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimIface.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimReg.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimReport.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimThreadTracker.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimCReg.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimIfaceTriv.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimReportControl.cpp 
  ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimThread.cpp)

set (HEADER_FILES 
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimCond.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimCReg.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimIface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimIfaceIvl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimIfaceSysc.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimIfaceTriv.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimReg.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimReportControl.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimReport.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimThread.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimThreadTracker.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/FrSimTime.h
  )

set (USE_IVL 0 CACHE BOOL "Use Icarus Verilog")

if (USE_IVL)
  set (SOURCE_FILES ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/src/FrSimIfaceIvl.cpp)
  include_directories(/usr/include/iverilog)
endif()

#set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)
#add_library(frsim STATIC ${SOURCE_FILES})
add_library(frsim SHARED ${SOURCE_FILES})
#target_link_libraries(frsim pth)

target_include_directories(frsim PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include/frsim>  # <prefix>/include/mylib
)

# add_subdirectory(tests)

set_target_properties(frsim PROPERTIES
  PUBLIC_HEADER "${HEADER_FILES}")

#install(TARGETS frsim
#  EXPORT frverificationTargets
#  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
#  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/frsim" COMPONENT dev)
install(TARGETS frsim
  EXPORT frverificationTargets
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/frsim" COMPONENT dev)

