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
 * Brief Description: A FRIc client model which can only originate transactions
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

module fric_client_master
  (
   clk,
   rst,

   fric_in,
   fric_out,

   // FIXME: shouldn't I prepend something to these?
   ctyp,
   port,
   addr,
   wdat,
   tstb, // transaction strobe -- initiate based on above
   trdy, // tstb may only be asserted when this is already high
   
   rstb, // reply strobe
   rdat // reply data
   );

   // In/Out declarations
   input           clk;
   input 	   rst;

   input [7:0] 	   fric_in;
   output [7:0]    fric_out;

   input [3:0] 	   ctyp;
   input [3:0] 	   port;
   input [7:0] 	   addr;
   input [15:0]    wdat;
   input 	   tstb;
   output 	   trdy;

   output 	   rstb;
   output [15:0]   rdat;

   // Parameters
   parameter [3:0] fos_idle = 4'h0,
		   fos_wr_adr = 4'h2,
		   fos_wr_da0 = 4'h3,
		   fos_wr_da1 = 4'h4,
		   fos_wr_ack = 4'h5,
		   fos_rd_adr = 4'h7,
		   fos_rd_ack = 4'h8;
   parameter [3:0] fis_idle = 4'h0,
		   fis_wr_ack = 4'h1,
		   fis_rd_ack = 4'h2,
		   fis_rd_da0 = 4'h3,
		   fis_rd_da1 = 4'h4;
   
   // Regs and Wires
   reg [3:0] 	   fos_state, next_fos_state;
   reg 		   trdy;
   reg [2:0] 	   fric_out_sel;
   reg [7:0] 	   fric_out;
   reg [3:0] 	   fis_state, next_fis_state;
   reg 		   rdat_cap_high;
   reg 		   rdat_cap_low;
   reg 		   next_rstb;
   reg 		   fis_ack;
   reg [7:0] 	   fric_inr;
   reg [15:0] 	   rdat;
   reg 		   rstb;
   

   // RTL

   /****************************************************************************
    * Fric Out Sequencer (fos)
    * 
    * This model will only send reads or writes out. It will never have to ack
    * anything because it only originates transactions and never responds to 
    * them.
    * 
    * The FSM is fairly simple. It either sends a 4 cycle write packet or a 2 
    * cycle read packet. trdy is deasserted as soon as it leaves idle. The fsm
    * doesn't return to idle until it gets notified of an ack from the fric in 
    * sequencer.
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * Note that the timing of rstb and trdy are depended on in the fric/pci 
    * interface. The rising edge of trdy must always be 1 cycle before rstb
    * 
    */
   always @ (/*AS*/ctyp or fis_ack or fos_state or tstb)
     begin
	// FSM default outputs
	next_fos_state = fos_state;
	trdy = 1'b0;
	fric_out_sel = 0;
	
	case(fos_state)
	  fos_idle:
	    begin
	       if(tstb==1'b1 && ctyp==4'b0010) begin
		  fric_out_sel = 1;
		  next_fos_state = fos_wr_adr;
	       end else if(tstb==1'b1 && ctyp==4'b0011) begin
		  fric_out_sel = 1;
		  next_fos_state = fos_rd_adr;
	       end else begin
		  trdy = 1'b1;
	       end
	    end
	  fos_wr_adr:
	    begin
	       fric_out_sel = 2;
	       next_fos_state = fos_wr_da0;
	    end
	  fos_wr_da0:
	    begin
	       fric_out_sel = 3;
	       next_fos_state = fos_wr_da1;
	    end
	  fos_wr_da1:
	    begin
	       fric_out_sel = 4;
	       next_fos_state = fos_wr_ack;
	    end
	  fos_wr_ack:
	    begin
	       fric_out_sel = 0;
	       if(fis_ack==1'b1) begin
		  trdy = 1'b1;
		  next_fos_state = fos_idle;
	       end
	    end
	  fos_rd_adr:
	    begin
	       fric_out_sel = 2;
	       next_fos_state = fos_rd_ack;
	    end
	  fos_rd_ack:
	    begin
	       fric_out_sel = 0;
	       if(fis_ack==1'b1) begin
		  trdy = 1'b1;
		  next_fos_state = fos_idle;
	       end
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

	     case(fric_out_sel)
	       3'h0: fric_out <= 8'h00;
	       3'h1: fric_out <= {ctyp, port};
	       3'h2: fric_out <= addr;
	       3'h3: fric_out <= wdat[7:0];
	       3'h4: fric_out <= wdat[15:8];
	       default: fric_out <= 8'h00;
	     endcase // case(fric_out_sel)
	     
	  end
     end // always @ (posedge clk)
   


   /****************************************************************************
    * Fric In Sequencer (fis)
    * 
    * This model will only get ACK packets in. It will never get a read or write
    * because it is only a master.
    * 
    * This machine simply waits for read or write acks. Any other packet ctyp is
    * an error. For a read ack, the rdat is registered and the rstb is asserted.
    * This model doesn't yet have any mechanism to support interrupts!
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * Note that the timing of rstb and trdy are depended on in the fric/pci 
    * interface. The rising edge of trdy must always be 1 cycle before rstb
    */
   always @ (/*AS*/fis_state or fric_inr)
     begin
	// FSM default outputs
	next_fis_state = fis_state;
	rdat_cap_high = 0;
	rdat_cap_low = 0;
	next_rstb = 0;
	fis_ack = 0;
	
	case(fis_state)
	  fis_idle:
	    begin
	       if(fric_inr[7:4]==4'b0100)
		 next_fis_state = fis_wr_ack;
	       else if(fric_inr[7:4]==4'b0101)
		 next_fis_state = fis_rd_ack;
	    end
	  fis_wr_ack:
	    begin
	       fis_ack = 1'b1;
	       next_fis_state = fis_idle;
	    end
	  fis_rd_ack:
	    begin
	       next_fis_state = fis_rd_da0;
	    end
	  fis_rd_da0:
	    begin
	       rdat_cap_low = 1'b1;
	       next_fis_state = fis_rd_da1;
	    end
	  fis_rd_da1:
	    begin
	       rdat_cap_high = 1'b1;
	       next_rstb = 1'b1;
	       fis_ack = 1'b1;
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
	     rdat <= 0;
	     rstb <= 0;
	  end
	else
	  begin
	     fis_state <= next_fis_state;
	     fric_inr <= fric_in;
	     if(rdat_cap_high==1'b1)
	       rdat[15:8] <= fric_inr;
	     if(rdat_cap_low==1'b1)
	       rdat[7:0] <= fric_inr;
	     rstb <= next_rstb;
	  end
     end // always @ (posedge clk)
   

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
   


endmodule // fric_client_slave



