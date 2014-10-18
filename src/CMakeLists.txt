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

set(CMAKE_C_FLAGS  "-O2 -finline -march=nocona -Wall -std=gnu99 -pedantic -g -rdynamic -DVAL_OVERLOAD")
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
