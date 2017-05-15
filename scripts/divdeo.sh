#!/bin/bash

# A script for dividing a video into shots
# @author skletz
# @version 1.0 15/05/07

VDO=""
CSV=""
USAGE="A tool for dividing a video into shots.
Usage: `basename $0` [-video] [-csv]
    -h    Shows this help
    -v    Video File
    -s    Csv File including shot boundaries: <start> <endframe>
 Examples:
    `basename $0` -v \"./flowers.mp4\" -s \"./flowers.cvs\""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts v:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    v)  VDO=$OPTARG ;;
    s)  CSV=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1` 

echo "Video File: $VDO; CSV File: $CSV"
