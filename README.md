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
* boost
* g++

Installation of prequisites on Ubuntu:

```
sudo apt-get install cmake cmake-gui libpth-dev iverilog boost g++
```

Currently frsimulation uses Cmake for a build system.  There are currently two cmake workspaces, the main workspace for building the libraries and a second workspace to build the example using Icarus Verilog.

Build and install libraries using the command-line:
```
mkdir build
cd build
cmake -D USE_IVL=1 -D CMAKE_INSTALL_PREFIX=<install location> <frsimulation_home>
make
make install
```

Build and install examples using the command-line:
```
mkdir build_ex
cd build_ex
cmake -D FRVM_PATH=<library install location> -D CMAKE_INSTALL_PREFIX=<install location> <frsimulation_home>/examplex/fric
make
make install
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<install location>
```

Run the example randTest:
```
cd <example install location>
vvp -M lib -m librandTest bin/fric_rtl
```


