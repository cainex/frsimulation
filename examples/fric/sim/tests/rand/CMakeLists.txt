include_directories(${SHARED_LIB_INCLUDE_DIR})
include_directories(${FRVM_PATH}/include)
include_directories(${FRVM_PATH}/include/frvm)
include_directories(${FRVM_PATH}/include/frsim)

set(SOURCE_FILES
  randTest.cpp)

include_directories(${FRVM_PATH}/include)
include_directories(${FRVM_PATH}/include/frvm)
include_directories(${FRVM_PATH}/include/frsim)
link_directories(${FRVM_PATH}/lib)

add_library(randTest SHARED ${SOURCE_FILES})
target_link_libraries(randTest fric frvm -Wl,--whole-archive frsim -Wl,--no-whole-archive pth )
set_target_properties(randTest PROPERTIES SUFFIX .vpi)

install(TARGETS randTest
  DESTINATION lib)
