#!/bin/bash

# A script to find missing shots based on extracted shot list and a directory containing the shots
# @author skletz
# @version 1.0 18/05/07

I1=""
I2=""
USAGE="A tool for dividing a video into shots.
Usage: `basename $0` [-csv] [-shots]
    -h    Shows this help
    -c    List shots: CSV File containing the filenames (csv)
    -s    Directory of shots
 Examples:
    `basename $0` -l \"./shots.csv\" -s \"./shots\""

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts c:s:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    c)  I1=$OPTARG ;;
    s)  I2=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

echo "List of Shots: $I1; Directory of Shots: $I2"
OUTPUTFILE=$(dirname "$I2")/missing-shots.csv
touch $OUTPUTFILE
INPUT=$I1

counter=0
IFS=","
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read shtfilename
do
  FILE=$I2/$shtfilename
  if [ ! -f "$FILE" ]; then
    echo "File not found: $I2/$shtfilename"
    echo "$shtfilename" >> $OUTPUTFILE
    counter=$((counter+1))
  else
    echo "File found: $I2/$shtfilename"
  fi

done < $INPUT
IFS=$OLDIFS

echo "$counter files not found"
