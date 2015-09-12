#****************************************************************************
# Copyright (c) 2015 by Daniel Grabowski.
#
# This file is part of FrSim.
# 
# FrSim is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# FrSim is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with FrSim.  If not, see <http://www.gnu.org/licenses/>.
#****************************************************************************

all : dump.vcd ;

../libfrsim.a: $(wildcard ../*.cpp ) $(wildcard ../*.h)
	make -C ..

ivltest.vvp : ivltest.v
	iverilog -o ivltest.vvp -s ivltest ivltest.v

dump.vcd : ivltest.vvp ivltest.vpi
	vvp -v -m ivltest -M . ivltest.vvp

waves: dump.vcd
	gtkwave dump.vcd

ivltest.o : ivltest.cpp
	g++ -o ivltest.o -fPIC -g -I.. -c ivltest.cpp 

# The simIfaceIV must be explicitly included in the library because the user test
# doesn't directly depend on it so it won't be included otherwise.
ivltest.vpi : ivltest.o ../libfrsim.a
	g++ ivltest.o ../FrSimIfaceIvl.o ../libfrsim.a -fPIC -g -lpth -shared -o ivltest.vpi

clean : 
	rm -f ivltest.vpi ivltest.o dump.vcd ivltest.vvp
	make -C .. clean


