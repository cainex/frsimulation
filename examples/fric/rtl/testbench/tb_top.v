/****************************************************************************
 * Copyright (c) 2009 by Focus Robotics. All rights reserved.
 *
 * This program is an unpublished work fully protected by the United States 
 * copyright laws and is considered a trade secret belonging to the copyright
 * holder. No part of this design may be reproduced stored in a retrieval 
 * system, or transmitted, in any form or by any means, electronic, 
 * mechanical, photocopying, recording, or otherwise, without prior written 
 * permission of Focus Robotics, Inc.
 *
 * Proprietary and Confidential
 *
 * Created By   :  Andrew Worcester
 * Creation_Date:  Fri Apr  3 2009
 * 
 * Brief Description: Top level testbench for FRIc client and switch testing.
 * 
 * Functionality:
 * 
 * Issues:
 * 
 * Limitations:
 * 
 * Testing:
 * 
 * Synthesis:
 * 
 ******************************************************************************/
`timescale 1ns/1ps

module tb_top();
   reg           clk, rst;
   wire [7:0] 	 mfric_out, mfric_in;
   wire [7:0] 	 sfric_out, sfric_in;

   wire [3:0] 	 m_type;
   wire [3:0] 	 m_port;
   wire [7:0] 	 m_addr;
   wire [15:0] 	 m_wdat;
   wire 	 m_tstb;
   wire 	 m_trdy;
   wire 	 m_rstb;
   wire [15:0] 	 m_rdat;

   wire [7:0] 	 s_addr;
   wire [15:0] 	 s_wdat;
   wire 	 s_wstb;
   wire [15:0] 	 s_rdat;

   wire [7:0] 	 p2_fric_out, p2_fric_in;
   wire [7:0] 	 p3_fric_out, p3_fric_in;
   wire [7:0] 	 p4_fric_out, p4_fric_in;
   wire [7:0] 	 p5_fric_out, p5_fric_in;
   wire [7:0] 	 p6_fric_out, p6_fric_in;
   wire [7:0] 	 p7_fric_out, p7_fric_in;

  initial $display("FRIC tb_top");

   initial begin
      clk <= 0;
      forever #7.5 clk <= ~clk;
   end

   initial begin
      rst = 1'b1;
      #120 rst = 1'b0;
   end

`ifdef DUMPS
  initial $display("Dumping enabled");
   initial $dumpvars;
`endif

   // DUT instances

   fric_switch_nosym_8port switch
     (
      .clk(clk),
      .rst(rst),
      .fric_in0(mfric_out),
      .fric_out0(mfric_in),
      .fric_in1(sfric_out),
      .fric_out1(sfric_in),
      .fric_in2(p2_fric_in),
      .fric_out2(p2_fric_out),
      .fric_in3(p3_fric_in),
      .fric_out3(p3_fric_out),
      .fric_in4(p4_fric_in),
      .fric_out4(p4_fric_out),
      .fric_in5(p5_fric_in),
      .fric_out5(p5_fric_out),
      .fric_in6(p6_fric_in),
      .fric_out6(p6_fric_out),
      .fric_in7(p7_fric_in),
      .fric_out7(p7_fric_out)
      );

   fric_client_master master
     (
      .clk(clk),
      .rst(rst),
      .fric_in(mfric_in),
      .fric_out(mfric_out),
      .ctyp(m_type),
      .port(m_port),
      .addr(m_addr),
      .wdat(m_wdat),
      .tstb(m_tstb),
      .trdy(m_trdy),
      .rstb(m_rstb),
      .rdat(m_rdat)
      );

   fric_client_slave slave
     (
      .clk(clk),
      .rst(rst),
      .fric_in(sfric_in),
      .fric_out(sfric_out),
      .addr(s_addr),
      .wdat(s_wdat),
      .wstb(s_wstb),
      .rdat(s_rdat)
      );

`ifdef FRSIM
  initial $display("Using FRSIM");
   initial $frsim_system;

   fric_client_xactor mx
     (
      .clk(clk),
      .rst(rst),
      .slave_addr(),
      .slave_wdat(),
      .slave_wstb(),
      .slave_rdat(),
      .master_type(m_type),
      .master_port(m_port),
      .master_addr(m_addr),
      .master_wdat(m_wdat),
      .master_tstb(m_tstb),
      .master_trdy(m_trdy),
      .master_rstb(m_rstb),
      .master_rdat(m_rdat)
      );
   
   fric_client_xactor sx
     (
      .clk(clk),
      .rst(rst),
      .slave_addr(s_addr),
      .slave_wdat(s_wdat),
      .slave_wstb(s_wstb),
      .slave_rdat(s_rdat),
      .master_type(),
      .master_port(),
      .master_addr(),
      .master_wdat(),
      .master_tstb(),
      .master_trdy(),
      .master_rstb(),
      .master_rdat()
      );

   fric_xactor px2
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p2_fric_out),
      .fric_out(p2_fric_in)
      );

   fric_xactor px3
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p3_fric_out),
      .fric_out(p3_fric_in)
      );

   fric_xactor px4
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p4_fric_out),
      .fric_out(p4_fric_in)
      );

   fric_xactor px5
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p5_fric_out),
      .fric_out(p5_fric_in)
      );

   fric_xactor px6
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p6_fric_out),
      .fric_out(p6_fric_in)
      );

   fric_xactor px7
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p7_fric_out),
      .fric_out(p7_fric_in)
      );

   fric_monitor pxMon0
     (
      .clk(clk),
      .rst(rst),
      .fric_in(mfric_out),
      .fric_out(mfric_in)
      );
   
   fric_monitor pxMon1
     (
      .clk(clk),
      .rst(rst),
      .fric_in(sfric_out),
      .fric_out(sfric_in)
      );
   
   fric_monitor pxMon2
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p2_fric_in),
      .fric_out(p2_fric_out)
      );
   
   fric_monitor pxMon3
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p3_fric_in),
      .fric_out(p3_fric_out)
      );
   
   fric_monitor pxMon4
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p4_fric_in),
      .fric_out(p4_fric_out)
      );
   
   fric_monitor pxMon5
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p5_fric_in),
      .fric_out(p5_fric_out)
      );
   
   fric_monitor pxMon6
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p6_fric_in),
      .fric_out(p6_fric_out)
      );
   
   fric_monitor pxMon7
     (
      .clk(clk),
      .rst(rst),
      .fric_in(p7_fric_in),
      .fric_out(p7_fric_out)
      );
   
   
`endif

endmodule // tb_top




