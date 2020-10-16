#!/bin/bash

shopt -s expand_aliases

alias sedr="sed -r 's/.*/    &/g'"

argv0="${0##*/}"

perror() {
    echo -n -e "$@" >&2
}

list_types() {
    local typee
    for typee in class header script source; do
        echo "${1}$typee" >&2
    done
}

print_usage() {
    perror "Usage:
    $argv0 type parameter [options]
    where 'type' can be one of:\n"
    list_types '        '
    
    printf "\n    $argv0 class
    $argv0 header fileName [-s] [header ...] [-l header ...]
    $argv0 [--]help|-h
    $argv0 script fileName [-t type] [-x permissions] [--|-c] [commandLine ...]
    $argv0 source fileName [--main] [-s] [header ...] [-l header ...]\n"
    
    printf "\n    If the first argument is 'header' or 'source', then create a header or source file with the default extensions .h or .cc, respectively.
For both files, an optional list of headers to include can be provided. If the '-s' switch is passed, subsequent arguments will be placed in an #include directive with \
the name of the file enclosed in angular braces. This is the default behavior. But if '-l' is passed, subsequent arguments will be enclosed in double quotations instead.\n"
    
    printf "    If 'script' is the first argument, then a script file is written to. The second parameter is the shell being invoked; \
if not provided it defaults to bash. Subsequent arguments are copied to the file on separate lines. Those arg the commands being executed \
by the script. If one of the parameters is '-x', the file is chmod'ed to the next parameter.\n"
}

# print usage
if [ "$1" = "" ]; then
    perror "No arguments provided\n"
    print_usage
    exit 1
fi

# basic options
type="$1"

case $type in
    class)
        ;;
    
    header)
        filename="${2:?no file specified}"
        if [ "${filename#*.}" = "" ]; then
            filename="$filename.h"
        fi
        
        # open and close braces
        openbrace='<'
        closebrace='>'
        
        # open file
        exec 3>"$filename"
        
        # create header guard name (e.g., temp.sh => TEMP_H)
        headertag="${filename##*/}"
        headertag="${headertag^^}"
        headertag="${headertag/./_}"
        
        printf "#ifndef $headertag\n#define $headertag\n" >&3
        
        shift 2
        
        # each subsequent argument is an #include
        for header in "$@"; do
            if [ "$header" = '-s' ]; then
                openbrace='<'
                closebrace='>'
                continue
            elif [ "$header" = '-l' ]; then
                openbrace='"'
                closebrace='"'
                continue
            fi
            
            echo "#include ${openbrace}$header${closebrace}" >&3
        done
        
        printf "\n\n#endif /* $headertag */" >&3
        exec 3>&-
        ;;
    
    help|--help|-h)
        print_usage
        exit 1
        ;;
    
    script)
        filename="${2:?no file specified}"
        shift 2
        type='bash'
        permissions=755
        
        let x=1
        
        # process options
        while getopts ":x:t:c" option "$@"; do
            case $option in
                t)
                    type="$OPTARG"
                    let ++x
                    let ++x
                    ;;
                
                x)
                    permissions=$OPTARG
                    let ++x
                    let ++x
                    ;;
                
                c)
                    let ++x
                    let stop=1
                    ;;
                
                :)
                    perror "Option '-$option' requires an argument"
                    exit 1
                    ;;
                
                *)
                    perror "Invalid option '-$OPTARG'\n"
                    exit 1
                    ;;
            esac
            
            if [ ${stop:=0} -eq 1 ]; then break; fi
        done
        
        # shift commandline to remove processed arguments
        shift $x
        
        # write to file
        exec 3>"$filename"
        
        echo "#!/bin/$type" >&3
        for line in "$@"; do
            echo "$line" >&3
        done
        
        # close file descriptor
        exec 3>&-
        
        chmod $permissions $filename
        ;;
    
    source)
        filename="${2:?no file specified}"
        shift 2
        
        # no extension
        if [ "${filename#*.}" = "" ]; then
            filename="$filename.cc"
        fi
        
        # open and close braces
        openbrace='<'
        closebrace='>'
        
        # open file
        exec 3>"$filename"
        
        # write to file
        for header in "$@"; do
            if [ "$header" = '-s' ]; then
                openbrace='<'
                closebrace='>'
                continue
            elif [ "$header" = '-l' ]; then
                openbrace='"'
                closebrace='"'
                continue
            elif [ "$header" = '--main' ]; then
                includemain=1
                continue
            fi
            
            echo "#include ${openbrace}$header${closebrace}" >&3
        done
        
        # write main function
        if [ -v includemain ]; then
            echo -e "\nint main(int argc, const char* argv[])\n{\n}" >&3
        fi
        
        # close file
        exec 3>&-
        ;;
    
    *)
        echo "Unknown type '$type'" >&2
        exit 1
        ;;
esac

if [ -n "$filename" ]; then
    echo "Created $filename"
fi
