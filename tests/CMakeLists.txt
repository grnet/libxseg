# Copyright (C) 2010-2014 GRNET S.A.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.8)

project (xseg_tests)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../src/include)

add_executable(xbinheap_test xbinheap_test.c)
target_link_libraries(xbinheap_test xseg)

add_executable(xcache_test xcache_test.c)
target_link_libraries(xcache_test xseg)

add_executable(xhash_test xhash_test.c)
target_link_libraries(xhash_test xseg)

add_executable(xheap_test xheap_test.c)
target_link_libraries(xheap_test xseg)

#add_executable(xpool_test xpool_test.c)
#target_link_libraries(xpool_test xseg)

add_executable(xobj_test xobj_test.c)
target_link_libraries(xobj_test xseg)

add_executable(xworkq_test xworkq_test.c)
target_link_libraries(xworkq_test xseg)

add_executable(xwaitq_test xwaitq_test.c)
target_link_libraries(xwaitq_test xseg)

add_executable(xq_test xq_test.c)
target_link_libraries(xq_test xseg m)

add_executable(xq_lock_test xq_lock_test.c)
target_link_libraries(xq_lock_test xseg m)
