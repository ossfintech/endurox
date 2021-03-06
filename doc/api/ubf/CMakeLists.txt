##
## @brief Build documentation, XATMI API
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

set(MAN3_NAMES 
    Bchg
    CBchg
    Berror
    B_error
    Bstrerror
    Blen
    Badd
    CBadd
    CBget
    Bget
    Bdel
    Bdelall
    Bpres
    Bproj
    Bprojcpy
    Bfldid
    Bfname
    Bcpy
    Binit
    Bnext
    Bboolco
    Bboolpr
    Bboolprcb
    Bfind
    CBfind
    Bboolev
    Bfloatev
    Bmkfldid
    Boccur
    Bused
    Bunused
    Bfldtype
    Bdelete
    Bfldno
    Bsizeof
    Btype
    Bfree
    Balloc
    Brealloc
    Bupdate
    CBfindocc
    Bfindocc
    CBgetalloc
    Bfindlast
    Bgetlast
    Bprint
    Bfprint
    Bfprintcb
    Btypcvt
    Bextread
    Bextreadcb
    Bread
    Breadcb
    Bwrite
    Bwritecb
    Btreefree
    Bboolsetcbf
    Badds
    Bchgs
    Bgets
    Bgetsa
    Bfinds
    Bisubf
    Bindex
    Bunindex
    Bidxused
    Brstrindex
    Bvstof
    Bvftos
    Bvselinit
    Bvsinit
    Bvopt
    Bvnull
    Bconcat
    CBvget
    CBvchg
    Bvsizeof
    Bvcpy
    Bvoccur
    Bvsetoccur
    Bvnext
    Baddfast
    Bcmp
    Bsubset
    Bfldddbgetenv
    Bflddbload
    Bflddbid
    Bflddbget
    Bflddbunlink
    Bflddbunload
    Bflddbdrop
    Bflddbdel
    Bflddbadd
    Bnum
    Bjoin
    Bojoin
   )
set(PDF_MAN_NAMES ${MAN3_NAMES})

# Generate MAN3
set(A2X_OPTS_MAN3 --format=manpage -D man3)
set(MAN3_FILES)
foreach(m IN LISTS MAN3_NAMES)
  set(mf ${CMAKE_CURRENT_BINARY_DIR}/man3/${m}.3)
  set(ms ${CMAKE_CURRENT_SOURCE_DIR}/${m}.adoc)
  add_custom_command(OUTPUT ${mf}
    COMMAND ${A2X_EXECUTABLE} ${A2X_OPTS_MAN3} ${ms}
    DEPENDS ${ms}
    WORKING_DIRECTORY .
    COMMENT "Building MAN 3 ${mf}"
    VERBATIM)
  list(APPEND MAN3_FILES ${mf})
  # Install the docs
  install (FILES
    ${mf}
    DESTINATION share/man/man3)

endforeach()
add_custom_target(man3_ubf ALL DEPENDS ${MAN3_FILES})

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
add_custom_target(pdf_man3_ubf ALL DEPENDS ${PDF_MAN_FILES})
endif()

# end.


# vim: set ts=4 sw=4 et smartindent:
