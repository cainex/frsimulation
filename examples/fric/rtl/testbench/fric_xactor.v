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
 * Creation_Date:  Mon Apr  6 2009
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

module fric_xactor
  (
   clk,
   rst,
   fric_in,
   fric_out
   );

   input           clk;
   input 	   rst;
   input [7:0] 	   fric_in;
   output [7:0]    fric_out;

   reg [7:0] 	   fric_out_o;
   wire [7:0] 	   fric_in_i;
   
   assign #1 	   fric_out = fric_out_o;
   assign #1 	   fric_in_i = fric_in;
   

endmodule // fric_xactor






