#!/bin/bash

shopt -s expand_aliases extglob
alias egrep='grep --color=auto -sno'

pattern="'// ${1:?provide a word}.*\$'"

rootdir=/home/john/programming/C++/git/dewpsi

error() {
    echo "$@" >&2
}

subdirs=(debug events ImGui matrices os)

select startdir in Dewpsi "Dewpsi/vendor/getopt" Sandbox
do
    case "$startdir" in
        Dewpsi)
            prefix="$rootdir/$startdir"

            # src
            eval egrep -e "$pattern" "$prefix/src/Dewpsi_*.h"\
                "$prefix/src/Dewpsi_*.cc"

            # platform/sdl
            eval egrep -e "$pattern" "$prefix/src/platform/sdl/Dewpsi_*.h"\
                "$prefix/src/platform/sdl/Dewpsi_*.cc"

            # src/*
            for subdir in ${subdirs[*]}; do
                eval egrep -e "$pattern" "$prefix/src/$subdir/Dewpsi_*.h"\
                    "$prefix/src/$subdir/Dewpsi_*.cc"
            done
            break
            ;;

        */getopt)
            prefix="$rootdir/$startdir"
            eval egrep -r -e "$pattern" "$prefix/*"
            break
            ;;

        Sandbox)
            prefix="$rootdir/$startdir"
            eval egrep -e "$pattern" "$prefix/src/*.[hc]"
            break
            ;;

        *)
            error "Invalid option"
            exit 1
            ;;
    esac
done
