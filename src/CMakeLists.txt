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


configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/xseg/version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/include/xseg/version.h)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
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
INSTALL(DIRECTORY include DESTINATION /usr FILES_MATCHING PATTERN "*.h")
INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/include DESTINATION /usr)
#install(FILES libxseg0.so DESTINATION $DESTDIR/usr/lib/)
#install(FILES libxseg0.so.${MAJOR} DESTINATION $DESTDIR/usr/lib/)
#install(FILES libxseg0.so.${MAJOR}.${MINOR} DESTINATION $DESTDIR/usr/lib/)
