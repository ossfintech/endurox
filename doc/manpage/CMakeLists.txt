##
## @brief Build documentation
##
## @file CMakeLists.txt
##
## -----------------------------------------------------------------------------
## Enduro/X Middleware Platform for Distributed Transaction Processing
## Copyright (C) 2009-2016, ATR Baltic, Ltd. All Rights Reserved.
## Copyright (C) 2017-2018, Mavimax, Ltd. All Rights Reserved.
## This software is released under one of the following licenses:
## AGPL or Mavimax's license for commercial use.
## -----------------------------------------------------------------------------
## AGPL license:
## 
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU Affero General Public License, version 3 as published
## by the Free Software Foundation;
##
## This program is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
## PARTICULAR PURPOSE. See the GNU Affero General Public License, version 3
## for more details.
##
## You should have received a copy of the GNU Affero General Public License along 
## with this program; if not, write to the Free Software Foundation, Inc., 
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## -----------------------------------------------------------------------------
## A commercial use license is available from Mavimax, Ltd
## contact@mavimax.com
## -----------------------------------------------------------------------------
##
cmake_minimum_required(VERSION 2.8)

find_program(A2X_EXECUTABLE NAMES a2x)
find_program(DBLATEX "dblatex")

set(MAN5_NAMES ex_env ndrxdebug.conf ndrxconfig.xml ex_envover q.conf
                viewfile tpcache.ini ubfdb.ini)

set(MAN8_NAMES xadmin ndrxd ud mkfldhdr tpbridge tpevsrv tmsrv
           tprecover cpmsrv tmqueue cconfsrv tpbrdcstsv uvlog xmemck
           viewc buildclient buildserver exencrypt exdecrypt libcryptohost
           edb_dump edb_copy edb_stat edb_load tpcachebtsv tpcached tpcachesv)
# Joined lists
#set(PDF_MAN_NAMES xadmin ndrxd ud mkfldhdr tpbridge tpevsrv 
#                ex_env ndrxdebug.conf ndrxconfig.xml ex_envover
#                tmsrv tprecover cpmsrv tmqueue q.conf)


set(PDF_MAN_NAMES ${MAN5_NAMES} ${MAN8_NAMES})


# Generate MAN5
set(A2X_OPTS_MAN5 --format=manpage -D man5)
set(MAN5_FILES)
foreach(m IN LISTS MAN5_NAMES)
  set(mf ${CMAKE_CURRENT_BINARY_DIR}/man5/${m}.5)
  set(ms ${CMAKE_CURRENT_SOURCE_DIR}/${m}.adoc)
  add_custom_command(OUTPUT ${mf}
    COMMAND ${A2X_EXECUTABLE} ${A2X_OPTS_MAN5} ${ms}
    DEPENDS ${ms}
    WORKING_DIRECTORY .
    COMMENT "Building MAN 5 ${mf}"
    VERBATIM)
  list(APPEND MAN5_FILES ${mf})
  # Install the docs
  install (FILES
    ${mf}
    DESTINATION share/man/man5)

endforeach()
add_custom_target(man5 ALL DEPENDS ${MAN5_FILES})

# Generate MAN8
set(A2X_OPTS_MAN8 --format=manpage -D man8)
set(MAN8_FILES)
foreach(m IN LISTS MAN8_NAMES)
  set(mf ${CMAKE_CURRENT_BINARY_DIR}/man8/${m}.8)
  set(ms ${CMAKE_CURRENT_SOURCE_DIR}/${m}.adoc)
  add_custom_command(OUTPUT ${mf}
    COMMAND ${A2X_EXECUTABLE} ${A2X_OPTS_MAN8} ${ms}
    DEPENDS ${ms}
    WORKING_DIRECTORY .
    COMMENT "Building MAN 8 ${mf}"
    VERBATIM)
  list(APPEND MAN8_FILES ${mf})

  # Install the docs
  install (FILES
    ${mf}
    DESTINATION share/man/man8)

endforeach()
add_custom_target(man8 ALL DEPENDS ${MAN8_FILES})

if(DBLATEX)
set(A2X_OPTS_PDF_MAN -L -f pdf --doctype book)
set(PDF_MAN_FILES)
foreach(m IN LISTS PDF_MAN_NAMES)
  set(mf ${CMAKE_CURRENT_BINARY_DIR}/${m}.pdf)
  set(ms ${CMAKE_CURRENT_SOURCE_DIR}/${m}.adoc)
  add_custom_command(OUTPUT ${mf}
    COMMAND ${A2X_EXECUTABLE} ${A2X_OPTS_PDF_MAN} ${ms}
    DEPENDS ${ms}
    WORKING_DIRECTORY .
    COMMENT "Building PDF_MAN ${mf}"
    VERBATIM)
  list(APPEND PDF_MAN_FILES ${mf})

  # Install the docs
  install (FILES
    ${mf}
    DESTINATION share/endurox/doc/pdf)

endforeach()
add_custom_target(pdf_man ALL DEPENDS ${PDF_MAN_FILES})
endif()

# end.


# vim: set ts=4 sw=4 et smartindent:
