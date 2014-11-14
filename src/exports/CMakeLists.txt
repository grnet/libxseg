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

project (xseg_exports)

SET(CMD ${CMAKE_CURRENT_SOURCE_DIR}/make_symbol_map.sh)
SET(OUT ${CMAKE_CURRENT_BINARY_DIR}/libxseg.map)
SET(EXPORTS ${CMAKE_CURRENT_SOURCE_DIR}/xseg_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xq_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xbinheap_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xcache_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xpool_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xhash_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xheap_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xobj_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xwaitq_exports.h
	    ${CMAKE_CURRENT_SOURCE_DIR}/xworkq_exports.h
	    )

ADD_CUSTOM_COMMAND( OUTPUT ${OUT}
	COMMAND ${CMD} ${EXPORTS}
	      # The following should be ${doxyfile} only but it
	      # will break the dependency.
	      # The optimal solution would be creating a
	      # custom_command for ${doxyfile} generation
	      # but I still have to figure out how...
#      MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
#                      DEPENDS project_targets ${doxy_extra_files}
	COMMENT "Generating libxseg map")

ADD_CUSTOM_TARGET( libmap DEPENDS ${out} )
