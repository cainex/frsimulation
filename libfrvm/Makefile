#***************************************************************************
# Copyright (c) 2015 by Daniel Grabowski.
#
# This file is part of FrVm.
# 
# FrVm is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# FrVm is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with FrVm.  If not, see <http://www.gnu.org/licenses/>.
#
# Created by: Daniel Grabowski
#
#****************************************************************************

include ../../etc/globals.mk

# should only need this for SimIfaceIvl which compiles separately, but deps complain
# if there is no veriuser or vpi_user in the include path
# FIXME: clean this up at some point! do deps properly, only compile needed files
INCLUDES 	= -I/usr/include/iverilog -I$(SANDBOX_ROOT)/src/libfrsim
#LDFLAGS 	+= -Wl,--rpath -Wl,$(OPENCV_LIBDIR)
LDLIBS		+= $(LIBFRSIM_LIBS)
CXXFLAGS	+= -fPIC -g
SOURCES	= ${wildcard *.cpp} 
SOURCE_DEPS = $(SOURCES:.cpp=.d)

LIBFRVM_SRCS = ${wildcard Fr*.cpp}
#LIBFRVM_SRCS = FrSimCReg.cpp FrSimCond.cpp FrSimIface.cpp FrSimIfaceTriv.cpp FrSimReg.cpp FrSimReport.cpp FrSimReportControl.cpp FrSimThread.cpp FrSimThreadTracker.cpp 
LIBFRVM_HDRS = ${wildcard Fr*.h}
LIBFRVM_OBJS = $(LIBFRVM_SRCS:%.cpp=%.o)
LIBFRVM_LIBS = libfrvm.a

TEST_SOURCES = ${wildcard test*.cpp}
TEST_OBJECTS = $(TEST_SOURCES:%.cpp=%.o)
TESTS = $(TEST_SOURCES:%.cpp=%)

.PHONY : all test doc install uninstall clean distclean

all: $(LIBFRVM_LIBS) $(TESTS) ;

lib: $(LIBFRVM_LIBS) ;

-include $(SOURCE_DEPS)

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CXXFLAGS) $(INCLUDES) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $<

FrSimIfaceVCS.o: FrSimIfaceVCS.cpp
	$(CXX) -I$(VCS_HOME)/include $(CXXFLAGS) $(INCLUDES) -c $<

FrSimIfaceIvl.o: FrSimIfaceIvl.cpp
	$(CXX) -I/usr/local/include -I/usr/include/iverilog $(CXXFLAGS) $(INCLUDES) -c $<

.PHONY : clean all

# Generate the library
$(LIBFRVM_LIBS): $(LIBFRVM_OBJS)
	rm -f $@
	ar cq $@ $(LIBFRVM_OBJS)

# Generate the tests
$(TESTS) : % : %.o $(LIBFRVM_LIBS)
	$(CXX) $^ $(LDFLAGS) $(LDLIBS) -o $@

# FIXME: this target should run tests, not just build them!
test: $(TESTS) ;

# Generate documentation
doc: Doxyfile ${SOURCES} $(SOURCE_DEPS)
	doxygen

install: $(LIBFRVM_LIBS)
	cp $(LIBFRVM_LIBS) $(LIBFRVM_INST)/lib
	cp $(LIBFRVM_HDRS) $(LIBFRVM_INST)/include

uninstall:
	rm -f $(LIBFRVM_LIBS:%=$(LIBFRVM_INST)/lib/%)
	rm -f $(LIBFRVM_HDRS:%=$(LIBFRVM_INST)/include/%)


clean:
	rm -rf *.o *.d *.d.* doc $(LIBFRVM_LIBS) $(TESTS)

distclean: uninstall clean ;



