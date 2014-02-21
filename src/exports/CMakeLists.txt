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
