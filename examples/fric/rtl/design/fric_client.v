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
 * Brief Description: Full client for the Focus Robotics Control Interconnect
 * that supports reads and writes made to us from another block as well as reads
 * and writes originating from this block made to another block.
 * 
 * Functionality:
 * 
 * Issues:
 * - The interface currently seems a little clumsy. I am hoping to clean it up
 * to be a little more compact and general as I work with it.
 * 
 * Limitations:
 * 
 * Testing:
 * 
 * Synthesis:
 * 
 ******************************************************************************/

module fric_client
  (
   clk,
   rst,

   fric_in,
   fric_out,

   slave_addr, // addr out for read or write requests sent to this block
   slave_wdat,
   slave_wstb,
   slave_rdat, // replay data in to reply to a read request made to us

   master_type,
   master_port,
   master_addr, // type, port, and addr in for reads or writes from this block
   master_wdat, // write data sent from this block
   master_tstb, // transaction strobe -- initiate based on above
   master_trdy, // flow control, tstb may only be asserted when this is already high
   
   master_rstb, // reply strobe out to alert block that reply has arrived
   master_rdat // reply data out

   );

   // Input/Output Declarations
   input           clk;
   input 	   rst;
   input [7:0] 	   fric_in;
   output [7:0]    fric_out;

   output [7:0]    slave_addr;
   output [15:0]   slave_wdat;
   output 	   slave_wstb;
   input [15:0]    slave_rdat;

   input [3:0] 	   master_type;
   input [3:0] 	   master_port;
   input [7:0] 	   master_addr;
   input [15:0]    master_wdat;
   input 	   master_tstb;
   output 	   master_trdy;
   output 	   master_rstb;
   output [15:0]   master_rday;
   
   // Parameter Declarations

   // Wire and Reg Declarations

   // RTL

   /****************************************************************************
    * Fric In Sequencer (fis)
    * 
    * The FSM in this subblock must accept any type of incoming packet, both new
    * requests from other blocks and acks for requests we have previously made.
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   

   /****************************************************************************
    * Fric Out Sequencer (fos)
    * 
    * The Fric output sequencer must be able to send both new requests and acks 
    * for requests previously made to us. 
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   

   /****************************************************************************
    * Fric Reply Generator (frg)
    * 
    * This subblock monitors requests made of us and forms the appropriate reply
    * packets. This subblock controls the slave interface.
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   

   /****************************************************************************
    * Fric Request Muxer (frm)
    * 
    * This subblock controls the master interface and allows both the master
    * interface and the FRG to send outgoing data to the fos.
    * 
    * Inputs:
    * 
    * Outputs:
    * 
    * Todo/Fixme:
    * 
    */
   

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
   

endmodule // fric_client



