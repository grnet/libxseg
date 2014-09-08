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

project(xseg_python)

set(VERSIONCMD ${CMAKE_CURRENT_SOURCE_DIR}/version.sh
	${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_CUSTOM_COMMAND (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/xseg/version.py
		    COMMAND ${VERSIONCMD}
		    #DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/xseg/version.py"
		   )
ADD_CUSTOM_TARGET (pyversion DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/xseg/version.py)

set(ENV "env")

#set(H2XML "h2xml")
set(H2XML_FLAGS "-c")
set(H2XML_DEPS "")
set(H2XML_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/include")
set(H2XML_INCLUDE_DIR2 "${CMAKE_CURRENT_BINARY_DIR}/../src/include")

set(H2XML_OUTPUT_XSEG "${CMAKE_CURRENT_BINARY_DIR}/xseg.xml")
set(H2XML_INPUT_XSEG "${CMAKE_CURRENT_SOURCE_DIR}/../src/include/xseg/xseg.h")
set(H2XML_OUTPUT_PROTOCOL "${CMAKE_CURRENT_BINARY_DIR}/protocol.xml")
set(H2XML_INPUT_PROTOCOL "${CMAKE_CURRENT_SOURCE_DIR}/../src/include/xseg/protocol.h")

#set(XML2PY "xml2py")
set(XML2PY_FLAGS "-d -k defst -c")
set(XML2PY_ENV "LD_LIBRARY_PATH=\"../src\"")

set(XML2PY_DEPS_XSEG "${H2XML_OUTPUT_XSEG}")
set(XML2PY_OUTPUT_XSEG "${CMAKE_CURRENT_BINARY_DIR}/xseg/xseg_api.py")
set(XML2PY_DEPS_PROTOCOL "${H2XML_OUTPUT_PROTOCOL}")
set(XML2PY_OUTPUT_PROTOCOL "${CMAKE_CURRENT_BINARY_DIR}/xseg/xprotocol.py")

set(XSEG_LIB "libxseg.so.${MAJOR}")
set(OUTPUT ${XML2PY_OUTPUT_PROTOCOL} ${XML2PY_OUTPUT_XSEG})
set(DEPS "")

set (SED1 sed -i -e 's/xseg_get_data_nonstatic.restype = STRING/xseg_get_data_nonstatic.restype = POINTER(c_char)/g' xseg/xseg_api.py)
set(SED2 sed -i -e 's/xseg_get_target_nonstatic.restype = STRING/xseg_get_target_nonstatic.restype = POINTER(c_char)/g' xseg/xseg_api.py)
set(SED3 sed -i -e 's/xseg_get_signal_desc_nonstatic.restype = STRING/xseg_get_signal_desc_nonstatic.restype = POINTER(c_char)/g' xseg/xseg_api.py)

set(SED4 sed -i -e 's/xseg_reply_map_scatterlist \\* 0/POINTER(xseg_reply_map_scatterlist)/g' xseg/xprotocol.py)
set(SED5 sed -i -e 's/xseg_create_map_scatterlist \\* 0/POINTER(xseg_create_map_scatterlist)/g' xseg/xprotocol.py)


SET(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)
SET(SETUP_PY ${CMAKE_CURRENT_BINARY_DIR}/setup.py)
SET(SETUP_PY_BUILD_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/build)

SET(DISTSETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/distribute_setup.py)
SET(DISTSETUP_PY ${CMAKE_CURRENT_BINARY_DIR}/distribute_setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY})
configure_file(${DISTSETUP_PY_IN} ${DISTSETUP_PY})

ADD_CUSTOM_COMMAND( OUTPUT ${H2XML_OUTPUT_XSEG} ${H2XML_OUTPUT_PROTOCOL} ${XML2PY_OUTPUT_PROTOCOL} ${XML2PY_OUTPUT_XSEG} ${SETUP_PY_BUILD_OUTPUT} ${CMAKE_CURRENT_BINARY_DIR}/xseg ${CMAKE_CURRENT_BINARY_DIR}/xseg.egg-info
	            COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/xseg
		    COMMAND touch ${CMAKE_CURRENT_BINARY_DIR}/xseg/__init__.py

		    COMMAND ${H2XML} ${H2XML_FLAGS} -I ${H2XML_INCLUDE_DIR} -I ${H2XML_INCLUDE_DIR2} -o ${H2XML_OUTPUT_XSEG} ${H2XML_INPUT_XSEG}
		    COMMAND ${ENV} ${XML2PY_ENV} ${XML2PY} ${H2XML_OUTPUT_XSEG} -d -k defst -c -l ${XSEG_LIB} -o ${XML2PY_OUTPUT_XSEG}
		    COMMAND ${SED1}
		    COMMAND ${SED2}
		    COMMAND ${SED3}

		    COMMAND ${H2XML} ${H2XML_FLAGS} -I ${H2XML_INCLUDE_DIR} -I ${H2XML_INCLUDE_DIR2} -o ${H2XML_OUTPUT_PROTOCOL} ${H2XML_INPUT_PROTOCOL}
		    COMMAND ${ENV} ${XML2PY_ENV} ${XML2PY}
		    ${H2XML_OUTPUT_PROTOCOL} -d -k defst -c -l ${XSEG_LIB} -o ${XML2PY_OUTPUT_PROTOCOL}

		    COMMAND ${SED4}
		    COMMAND ${SED5}


		    COMMAND pwd
		    COMMAND python ${SETUP_PY} build

		    #		    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/xseg/version.py
		    #DEPENDS version
		    #                    DEPENDS ${DEPS}
		    COMMENT "Generating python bindings")

ADD_CUSTOM_TARGET(python ALL DEPENDS ${OUTPUT} )
ADD_DEPENDENCIES(python xseg)
ADD_DEPENDENCIES(python pyversion)

install(CODE "execute_process(COMMAND python ${SETUP_PY} install --install-layout=deb --root=$DESTDIR)")


