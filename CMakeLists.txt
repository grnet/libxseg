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

project (xseg)
SET(MAJOR 0)
SET(MINOR 2)


FIND_PROGRAM(H2XML h2xml)
IF(H2XML STREQUAL "H2XML-NOTFOUND")
    MESSAGE(FATAL_ERROR "h2xml is needed to build python bindings.")
ENDIF()
MESSAGE(STATUS "Found h2xml: " ${H2XML})

FIND_PROGRAM(XML2PY xml2py)
IF(XML2PY STREQUAL "XML2PY-NOTFOUND")
    MESSAGE(FATAL_ERROR "h2xml is needed to build python bindings.")
ENDIF()
MESSAGE(STATUS "Found xml2py: " ${XML2PY})


add_subdirectory(src)
add_subdirectory(python)
add_subdirectory(tests)

add_custom_target(build)
add_dependencies(build src python tests)
