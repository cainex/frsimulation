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
 * Creation_Date:  Tue Mar 10 2009
 * 
 * Brief Description:
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

module fric_switch_nosym_8port
  (
   clk,
   rst,
   fric_in0,
   fric_out0,
   fric_in1,
   fric_out1,
   fric_in2,
   fric_out2,
   fric_in3,
   fric_out3,
   fric_in4,
   fric_out4,
   fric_in5,
   fric_out5,
   fric_in6,
   fric_out6,
   fric_in7,
   fric_out7
   );

   // In/Out declarations
   input           clk;
   input 	   rst;
   input [7:0] 	   fric_in0;
   output [7:0]    fric_out0;
   input [7:0] 	   fric_in1;
   output [7:0]    fric_out1;
   input [7:0] 	   fric_in2;
   output [7:0]    fric_out2;
   input [7:0] 	   fric_in3;
   output [7:0]    fric_out3;
   input [7:0] 	   fric_in4;
   output [7:0]    fric_out4;
   input [7:0] 	   fric_in5;
   output [7:0]    fric_out5;
   input [7:0] 	   fric_in6;
   output [7:0]    fric_out6;
   input [7:0] 	   fric_in7;
   output [7:0]    fric_out7;

   // Parameters
   parameter [3:0] mp_idle = 4'h0,
		   mp_req_adr = 4'h1,
		   mp_wdat0 = 4'h2,
		   mp_wdat1 = 4'h3,
		   mp_wack = 4'h4,
		   mp_rep_adr = 4'h5,
		   mp_rdat0 = 4'h6,
		   mp_rdat1 = 4'h7,
		   mp_done = 4'hf;

   // Regs and Wires
   reg [3:0] 	   mp_state, next_mp_state;
   reg [7:0] 	   fric_out0;
   reg [7:0] 	   fric_inr0;
   reg [7:0] 	   pdat;
   reg [3:0] 	   psel, type, psel_reg;
   reg 		   capt_psel;
   reg 		   chng_port_out;
   reg 		   chng_port_in;
   
   reg [7:0] 	   prep;
   reg [7:0] 	   fric_out1;
   reg [7:0] 	   fric_out2;
   reg [7:0] 	   fric_out3;
   reg [7:0] 	   fric_out4;
   reg [7:0] 	   fric_out5;
   reg [7:0] 	   fric_out6;
   reg [7:0] 	   fric_out7;
   reg [7:0] 	   fric_inr1;
   reg [7:0] 	   fric_inr2;
   reg [7:0] 	   fric_inr3;
   reg [7:0] 	   fric_inr4;
   reg [7:0] 	   fric_inr5;
   reg [7:0] 	   fric_inr6;
   reg [7:0] 	   fric_inr7;
   

   // RTL or Instances

   /****************************************************************************
    * Subblock: Master Port (port0)
    * 
    * This is the only port which will be receiving requests from it's client.
    * Other ports will forward those requests to their clients and get and ack
    * back from them.
    * 
    * The FSM basically works like this: monitor fric_in0 upper 4 bits until
    * they are non-zero. Select the output port from the lower 4 bits of that
    * word. Overwrite the lower 4 bits of that word with 0's and send that to 
    * the output port. Send the next three words to that same output port and 
    * wait for the ack. So input port had to be selected the same as the output
    * port. Forward the ack to fric_out0, overwriting the lower 4 bits of the 
    * first word with the selected port. Start again.
    * 
    * Inputs: fric_in0, preply
    * 
    * Outputs: psel, pdat
    * 
    * Todo/Fixme:
    * 
    */
   always @ (/*AS*/fric_inr0 or mp_state or prep or type)
     begin
	// FSM default outputs
	next_mp_state = mp_state;
	capt_psel = 0;
	chng_port_out = 0;
	chng_port_in = 0;
	
	case(mp_state)
	  mp_idle: // wait for transaction to start then capture and send type and port
	    if(|fric_inr0[7:4]!=0) begin
	       capt_psel = 1'b1;
	       chng_port_out = 1'b1;
	       next_mp_state = mp_req_adr;
	    end

	  mp_req_adr: // send addr, go to wait ack if read, go to dat if write
	    if(type==4'b0010)
	      next_mp_state = mp_wdat0;
	    else if(type==4'b0011)
	      next_mp_state = mp_wack;
	    else
	      $display("Illegal type captured in fric switch nosym\n");

	  mp_wdat0:
	    next_mp_state = mp_wdat1;

	  mp_wdat1:
	    next_mp_state = mp_wack;

	  mp_wack:
	    // wait for ack to start and then send type, port
	    // change port value back from this port to slave port
	    // FIXME: should probably re-capture type instead of relying on 
	    // the ack to be correct based on the request.
	    // FIXME: with quick turnaround, is it possible for the ack to
	    // start before we get to this state?!?! probably not...
	    if(|prep[7:0]!=0) begin
	       chng_port_in = 1'b1;
	       next_mp_state = mp_rep_adr;
	    end

	  mp_rep_adr:
	    // send reply addr, got to idle if this is a write ack or continue
	    // for a read ack
	    if(type==4'b0010) // write ack only 2 cycles
	      next_mp_state = mp_idle;
	    else
	      next_mp_state = mp_rdat0;

	  mp_rdat0:
	    next_mp_state = mp_rdat1;

	  mp_rdat1:
	    next_mp_state = mp_idle;

	  default: next_mp_state = mp_idle;

	endcase // case(mp_state)
     end // always @ (...

   always @ (posedge clk)
     begin
	if(rst==1'b1)
	  begin
	     mp_state <= mp_idle;
	     fric_inr0 <= 0;
	     psel_reg <= 0;
	     type <= 0;
	     fric_out0 <= 0;
	     
	  end
	else
	  begin
	     mp_state <= next_mp_state;

	     fric_inr0 <= fric_in0;
	     
	     if(capt_psel==1'b1) begin
		psel_reg <= fric_inr0[3:0];
		type <= fric_inr0[7:4];
	     end

	     if(chng_port_in==1'b1)
	       fric_out0 <= {prep[7:4], psel};
	     else
	       fric_out0 <= prep;
	     
	  end
     end // always @ (posedge clk)

   always @ (/*AS*/capt_psel or chng_port_out or fric_inr0 or psel_reg)
     begin
	if(chng_port_out==1'b1) 
	  pdat <= {fric_inr0[7:4], 4'h0};
	else 
	  pdat <= fric_inr0;

	if(capt_psel==1'b1)
	  psel = fric_inr0[3:0];
	else
	  psel = psel_reg;
	
     end

   /****************************************************************************
    * Subblock: Slave Ports (ports 1-7)
    * 
    * Note that the various slave fric_out ports aren't driven to zero
    * explicitly so each transaction must go to zeroes before another one starts
    * 
    * Inputs: 
    * psel, pdat: from master port
    * fric_in1 - fric_in7
    * 
    * Outputs: prep: from reply port back to master
    * fric_out1 - fric_out7
    * 
    * Todo/Fixme:
    * 
    */
   always @ (posedge clk)
     begin
	if(rst==1'b1)
	  begin
	     prep <= 0;
	     fric_inr1 <= 0;
	     fric_inr2 <= 0;
	     fric_inr3 <= 0;
	     fric_inr4 <= 0;
	     fric_inr5 <= 0;
	     fric_inr6 <= 0;
	     fric_inr7 <= 0;
	     fric_out1 <= 0;
	     fric_out2 <= 0;
	     fric_out3 <= 0;
	     fric_out4 <= 0;
	     fric_out5 <= 0;
	     fric_out6 <= 0;
	     fric_out7 <= 0;
	  end
	else
	  begin
	     fric_inr1 <= fric_in1;
	     fric_inr2 <= fric_in2;
	     fric_inr3 <= fric_in3;
	     fric_inr4 <= fric_in4;
	     fric_inr5 <= fric_in5;
	     fric_inr6 <= fric_in6;
	     fric_inr7 <= fric_in7;
	     
	     case(psel)
	       3'b000: ;
	       3'b001: fric_out1 <= pdat;
	       3'b010: fric_out2 <= pdat;
	       3'b011: fric_out3 <= pdat;
	       3'b100: fric_out4 <= pdat;
	       3'b101: fric_out5 <= pdat;
	       3'b110: fric_out6 <= pdat;
	       3'b111: fric_out7 <= pdat;
	     endcase // case(psel)
	  end // else: !if(rst==1'b1)
     end // always @ (posedge clk)

   always @ (/*AS*/fric_inr1 or fric_inr2 or fric_inr3 or fric_inr4 or fric_inr5
	     or fric_inr6 or fric_inr7 or psel)
     begin
	case(psel)
	  3'b000: prep = 0;
	  3'b001: prep = fric_inr1;
	  3'b010: prep = fric_inr2;
	  3'b011: prep = fric_inr3;
	  3'b100: prep = fric_inr4;
	  3'b101: prep = fric_inr5;
	  3'b110: prep = fric_inr6;
	  3'b111: prep = fric_inr7;
	endcase // case(psel)
     end // always @ (...


   /****************************************************************************
    * Subblock
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   

endmodule // fric_switch_8port





