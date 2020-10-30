#!/usr/bin/env tclsh
load "./libddb.so"

set fileheader [ddb init]
set colheader [ddb columns $fileheader {{Name 30 S} {Element 10 S}}]

ddb print $colheader

set records [ddb records $fileheader $colheader {Angie Lightning} {Loyla Darkness}]

ddb print $records $colheader 0
