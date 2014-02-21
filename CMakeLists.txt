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
add_dependencies(build xseg python tests)
