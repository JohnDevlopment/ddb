#!/usr/bin/env tclsh
load "./libddb.so"

set fileheader [ddb init]

set fileheaderCopy $fileheader

unset fileheader
after 2000
unset fileheaderCopy
