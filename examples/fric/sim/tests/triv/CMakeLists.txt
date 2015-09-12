include_directories(${SHARED_LIB_INCLUDE_DIR})
include_directories(${FRVM_PATH}/include)
include_directories(${FRVM_PATH}/include/frvm)
include_directories(${FRVM_PATH}/include/frsim)

set(SOURCE_FILES
  trivTest.cpp)

include_directories(${FRVM_PATH}/include)
include_directories(${FRVM_PATH}/include/frvm)
include_directories(${FRVM_PATH}/include/frsim)
link_directories(${FRVM_PATH}/lib)

add_library(trivTest SHARED ${SOURCE_FILES})
target_link_libraries(trivTest fric frvm -Wl,--whole-archive frsim -Wl,--no-whole-archive pth )
set_target_properties(trivTest PROPERTIES SUFFIX .vpi)

install(TARGETS trivTest
  DESTINATION lib)

