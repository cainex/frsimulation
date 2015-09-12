if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(warnings "-Wall -Wextra -Werror")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set (SOURCE_FILES
  FricChecker.cpp
  FricData.cpp
  FricEnv.cpp
  FricIf.cpp
  FricMasterGen.cpp
  FricMasterIf.cpp
  FricMasterXactor.cpp
  FricMonitor.cpp
  FricResponseGen.cpp
  FricSlaveIf.cpp
  FricSlaveXactor.cpp
  FricXactor.cpp)

set (HEADER_FILES
  FricChecker.h
  FricData.h
  FricEnv.h
  FricIf.h
  FricMasterGen.h
  FricMasterIf.h
  FricMasterXactor.h
  FricMonitor.h
  FricResponseGen.h
  FricSlaveIf.h
  FricSlaveXactor.h
  FricXactor.h)

set (SHARED_LIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (SHARED_LIB_INCLUDE_DIR ${SHARED_LIB_INCLUDE_DIR} PARENT_SCOPE)

set (FRVM_PATH /usr/lib CACHE PATH "Path of libfrm library")

include_directories(${FRVM_PATH}/include)
include_directories(${FRVM_PATH}/include/frvm)
include_directories(${FRVM_PATH}/include/frsim)
link_directories(${FRVM_PATH}/lib)

set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fPIC)

add_library(fric STATIC ${SOURCE_FILES})
#target_link_libraries(fric pth)

