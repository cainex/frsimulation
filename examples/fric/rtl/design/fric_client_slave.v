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

module fric_client_slave
  (
   clk,
   rst,

   fric_in,
   fric_out,

   // FIXME: shouldn't I prepend something to these?
   addr,
   wdat,
   wstb,
   rdat
   );

   // In/Out declarations
   input           clk;
   input 	   rst;
   input [7:0] 	   fric_in;
   output [7:0]    fric_out;
   output [7:0]    addr;
   output [15:0]   wdat;
   output 	   wstb;
   input [15:0]    rdat;

   // Parameters
   parameter [3:0] fis_idle = 4'h0,
		   fis_rd_adr = 4'h1,
		   fis_wr_adr = 4'h2,
		   fis_wr_da0 = 4'h3,
		   fis_wr_da1 = 4'h4;
   parameter [3:0] fos_idle = 4'h0,
		   fos_rd_ack = 4'h1,
		   fos_rd_adr = 4'h2,
		   fos_rd_da0 = 4'h3,
		   fos_rd_da1 = 4'h4,
		   fos_wr_ack = 4'h5,
		   fos_wr_adr = 4'h6;

   // Regs and Wires
   reg [3:0] 	   fis_state, next_fis_state;
   reg 		   capt_port;
   reg 		   capt_addr;
   reg 		   capt_wdat_low;
   reg 		   capt_wdat_high;
   reg 		   wstb, wstb_pre;
   reg 		   init_rdack;
   reg 		   init_wrack;
   reg [7:0] 	   fric_inr;
   reg [7:0] 	   addr;
   reg [15:0] 	   wdat;
   reg [3:0] 	   port;
   reg [3:0] 	   fos_state, next_fos_state;
   reg [2:0] 	   fos_out_sel;
   reg [7:0] 	   fric_out;
   
   // RTL

   /****************************************************************************
    * Fric In Sequencer (fis)
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   always @ (/*AS*/fis_state or fric_inr)
     begin
	// FSM default outputs
	next_fis_state = fis_state;
	capt_port = 0;
	capt_addr = 0;
	capt_wdat_low = 0;
	capt_wdat_high = 0;
	wstb_pre = 0;
	init_rdack = 0;
	init_wrack = 0;
	
	case(fis_state)
	  fis_idle:
	    begin
	       if(fric_inr[7:4]==4'b0010) begin
		  capt_port = 1'b1;
		  next_fis_state = fis_wr_adr;
	       end else if(fric_inr[7:4]==4'b0011) begin
		  capt_port = 1'b1;
		  next_fis_state = fis_rd_adr;
	       end
	    end
	  fis_rd_adr:
	    begin
	       capt_addr = 1'b1;
	       init_rdack = 1'b1;
	       next_fis_state = fis_idle;
	    end
	  fis_wr_adr:
	    begin
	       capt_addr = 1'b1;
	       init_wrack = 1'b1;
	       next_fis_state = fis_wr_da0;
	    end
	  fis_wr_da0:
	    begin
	       capt_wdat_low = 1'b1;
	       next_fis_state = fis_wr_da1;
	    end
	  fis_wr_da1:
	    begin
	       capt_wdat_high = 1'b1;
	       wstb_pre = 1'b1;
	       next_fis_state = fis_idle;
	    end
	  default:
	    begin
	    end
	endcase // case(fis_state)
     end // always @ (...

   always @ (posedge clk)
     begin
	if(rst==1'b1)
	  begin
	     fis_state <= fis_idle;
	     fric_inr <= 0;
	     port <= 0;
	     addr <= 0;
	     wdat <= 0;
	     wstb <= 0;
	     
	  end
	else
	  begin
	     fis_state <= next_fis_state;
	     fric_inr <= fric_in;
	     if(capt_port==1'b1)
	       port <= fric_inr[3:0];
	     if(capt_addr==1'b1)
	       addr <= fric_inr;
	     if(capt_wdat_low==1'b1)
	       wdat[7:0] <= fric_inr;
	     if(capt_wdat_high==1'b1)
	       wdat[15:8] <= fric_inr;
	     wstb <= wstb_pre;
	     
	  end
     end // always @ (posedge clk)
   

   /****************************************************************************
    * Fric Out Sequencer
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   always @ (/*AS*/fos_state or init_rdack or init_wrack)
     begin
	// FSM default outputs
	next_fos_state = fos_state;
	fos_out_sel = 0;
	
	case(fos_state)
	  fos_idle:
	    begin
	       if(init_rdack==1'b1) begin
		  fos_out_sel = 2;
		  next_fos_state = fos_rd_adr;
	       end else if(init_wrack==1'b1) begin
		  fos_out_sel = 1;
		  next_fos_state = fos_wr_adr;
	       end
	    end
	  fos_rd_ack:
	    begin
	    end
	  fos_rd_adr:
	    begin
	       fos_out_sel = 3;
	       next_fos_state = fos_rd_da0;
	    end
	  fos_rd_da0:
	    begin
	       fos_out_sel = 4;
	       next_fos_state = fos_rd_da1;
	    end
	  fos_rd_da1:
	    begin
	       fos_out_sel = 5;
	       next_fos_state = fos_idle;
	    end
	  fos_wr_ack:
	    begin
	    end
	  fos_wr_adr:
	    begin
	       fos_out_sel = 3;
	       next_fos_state = fos_idle;
	    end
	  default:
	    begin
	    end
	endcase // case(fos_state)
     end // always @ (...

   always @ (posedge clk)
     begin
	if(rst==1'b1)
	  begin
	     fos_state <= fos_idle;
	     fric_out <= 0;
	     
	  end
	else
	  begin
	     fos_state <= next_fos_state;
	     case(fos_out_sel)
	       3'h0: fric_out <= 0;
	       3'h1: fric_out <= {4'b0100, port};
	       3'h2: fric_out <= {4'b0101, port};
	       3'h3: fric_out <= addr;
	       3'h4: fric_out <= rdat[7:0];
	       3'h5: fric_out <= rdat[15:8];
	       default: fric_out <= 0;
	     endcase // case(fos_out_sel)
	  end
     end // always @ (posedge clk)
   

   /****************************************************************************
    * Fric Reply Generator
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   


endmodule // fric_client_slave



