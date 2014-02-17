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

project (xseg_lib)

SET(OUT ${CMAKE_CURRENT_BINARY_DIR}/libxseg.map)
SET(EXPORTS ${CMAKE_CURRENT_SOURCE_DIR}/exports/xseg_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xq_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xbinheap_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xcache_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xpool_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xhash_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xheap_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xobj_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xwaitq_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/exports/xworkq_exports.h
	    )
SET(CMD cat ${EXPORTS} | ${CMAKE_CURRENT_SOURCE_DIR}/exports/make_symbol_map.sh > ${OUT})

ADD_CUSTOM_COMMAND( OUTPUT ${OUT}
	COMMAND ${CMD}
	      # The following should be ${doxyfile} only but it
	      # will break the dependency.
	      # The optimal solution would be creating a
	      # custom_command for ${doxyfile} generation
	      # but I still have to figure out how...
#        MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
#        DEPENDS xseg_lib ${OUT}
	COMMENT "Generating libxseg map")

#ADD_CUSTOM_TARGET( libmap DEPENDS ${out} )
ADD_CUSTOM_TARGET( libmap DEPENDS ${OUT} )
#ADD_CUSTOM_TARGET( libmap )


set(CMAKE_C_FLAGS  "-O2 -finline -march=nocona -Wall -std=gnu99 -pedantic -g -rdynamic -DVAL_OVERLOAD -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/libxseg.map")
#add_subdirectory(exports)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
set(xseg_srcs xseg.c initialize.c xseg_posix.c xseg_pthread.c xseg_posixfd.c
	xseg_user.c xtypes/xcache.c xtypes/xbinheap.c xtypes/xhash.c
	xtypes/xheap.c xtypes/xobj.c xtypes/xpool.c xtypes/xq.c xtypes/xwaitq.c
	xtypes/xworkq.c)
add_library(xseg SHARED ${xseg_srcs})
target_link_libraries(xseg rt pthread dl)

add_executable(xseg-tool xseg-tool.c)
target_link_libraries(xseg-tool xseg)

ADD_DEPENDENCIES(xseg libmap)
SET_TARGET_PROPERTIES(
	xseg
	PROPERTIES
	SOVERSION ${MAJOR}
	VERSION ${MAJOR}.${MINOR}
)

SET_TARGET_PROPERTIES(
	xseg-tool
	PROPERTIES
	OUTPUT_NAME xseg
)

INSTALL_TARGETS(/lib xseg)
INSTALL_TARGETS(/bin xseg-tool)
#INSTALL_FILES(/include )
INSTALL(DIRECTORY include DESTINATION /usr)
#install(FILES libxseg0.so DESTINATION $DESTDIR/usr/lib/)
#install(FILES libxseg0.so.${MAJOR} DESTINATION $DESTDIR/usr/lib/)
#install(FILES libxseg0.so.${MAJOR}.${MINOR} DESTINATION $DESTDIR/usr/lib/)
