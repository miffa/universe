#!/bin/sh

# template.sh
#   unix1986@qq.com's template style

PROG_NAME=`basename $0`
#global variable for command line arguments
#TODO

function usage() {
    #TODO
    return 0
}

function parse_args() {
    opt_args=`getopt -o "h" -l "help" -n "PROG_NAME" -- "$@"`
    [ $? -ne 0 ] && usage && exit 1
    eval set -- "$opt_args"
    #TODO
}

function main() {
    parse_args $@
    #TODO
}

main $@
