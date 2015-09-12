FreeRange RTL Verification Library
===========================================================

The FreeRange RTL Verification Library is a set of C++ libraries that can be used to add more advanced verification capabilities to various RTL simulators (currently supporting [Icarus Verilog](http://iverilog.icarus.com/)).  This is achieved using two different libraries:

* libfrsim - simulation support library
* libfrvm - verification methodology library

libfrsim
--------

The simulation support library, adds several key elements to C++ verification environment that are required for a modern, full-featured verification environment:

* 4-state vector data types
* Time processing
* Threading
* Connectivity to the simulator

libfrvm
-------

Ther verification methodology library builds on top of libfrsim and provides an verification API that closely follows the concepts found in VMM.

* Callbacks
* Channels
* Consensus
* Constraints
* Data classes
* Top-level environment with phasing
* Voter
* Transactors

Installation
------------
The FreeRange RTL Verification Library requires:

* cmake
* pth
* iverilog (optionally)

