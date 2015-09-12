set (TESTBENCH_SRC
  )

set (TESTBENCH_TOP
  ${CMAKE_CURRENT_SOURCE_DIR}/testbench/tb_top.v)

set (DESIGN_SRC
  )

set (RTL_OUTPUT
  fric_rtl
  )

set (IVERILOG_FLAGS
   -y ${CMAKE_CURRENT_SOURCE_DIR}/design -y ${CMAKE_CURRENT_SOURCE_DIR}/testbench -D FRSIM=1 -D DUMPS=1 -o ${RTL_OUTPUT}
  )

#iverilog -y /home/danielgr/projects/frip/fric/rtl/design -y /home/danielgr/projects/frip/fric/rtl/testbench -D FRSIM=1 -D DUMPS=1 /home/danielgr/projects/frip/fric/rtl/testbench/tb_top

add_custom_target(
  ${RTL_OUTPUT} ALL iverilog ${IVERILOG_FLAGS} ${TESTBENCH_TOP}
  DEPENDS ${TESTBENCH_SRC} ${TESTBENCH_TOP} ${DESIGN_SRC})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/fric_rtl
  DESTINATION bin)

  
  
