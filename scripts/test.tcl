#!/usr/bin/env tclsh
load "./libddb.so"

set fileheader [ddb init]
set colheader [ddb columns $fileheader {{Name 30 S} {Element 1 S}}]

puts "File header"
ddb print $fileheader

puts "\nColumns"
ddb print $colheader
