# Copyright 2014 GRNET S.A. All rights reserved.
#
# Redistribution and use in source and binary forms, with or
# without modification, are permitted provided that the following
# conditions are met:
#
#   1. Redistributions of source code must retain the above
#      copyright notice, this list of conditions and the following
#      disclaimer.
#
#   2. Redistributions in binary form must reproduce the above
#      copyright notice, this list of conditions and the following
#      disclaimer in the documentation and/or other materials
#      provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY GRNET S.A. ``AS IS'' AND ANY EXPRESS
# OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GRNET S.A OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
# USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# The views and conclusions contained in the software and
# documentation are those of the authors and should not be
# interpreted as representing official policies, either expressed
# or implied, of GRNET S.A.

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
