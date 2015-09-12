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
#
# Created by: Daniel Grabowski, Andrew Worcester
#
#****************************************************************************

add_executable(testFrReg testFrReg.cpp)
target_link_libraries(testFrReg frsim)

add_executable(testFrSimIfaceTriv testFrSimIfaceTriv.cpp)
target_link_libraries(testFrSimIfaceTriv frsim)

add_executable(testFrSimThread testFrSimThread.cpp)
target_link_libraries(testFrSimThread frsim)

add_test(testFrReg testFrReg)
add_test(testFrSimIfaceTriv testFrSimIfaceTriv)
add_test(testFrSimThread testFrSimThread)

