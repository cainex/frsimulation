include_directories(${SHARED_LIB_INCLUDE_DIR})

set(SOURCE_FILES
  trivTest.cpp)

link_directories(${FRVM_PATH}/lib)

find_package(frverification PATHS ${FRV_PATH} NO_CMAKE_PACKAGE_REGISTRY NO_CMAKE_ENVIRONMENT_PATH)

if (frverification_FOUND)
  message(STATUS "Found FRVerification Library!")
  include_directories(${frverification_INCLUDE_DIRS})
endif()

add_library(trivTest SHARED ${SOURCE_FILES})
target_link_libraries(trivTest fric frvm -Wl,--whole-archive frsim -Wl,--no-whole-archive pth )
set_target_properties(trivTest PROPERTIES SUFFIX .vpi)

install(TARGETS trivTest
  DESTINATION lib)

