#!/bin/bash

# A script to find missing shots based on extracted shot list and a directory containing the shots
# @author skletz
# @version 1.0 18/05/07

INPUT=""
OUTPUT=""

USAGE="A tool for dividing a video into shots.
Usage: `basename $0` [-csv] [-shots]
    -h    Shows this help
    -i    Input files csv <vdo, msb, id, start, end, duration, fps>
    -o    Output File
 Examples:
    `basename $0` -i shots1.csv -i shots2.csv -o shots.csv"

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts i:o:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    i)  INPUT+=("$OPTARG") ;;
    o)  OUTPUT=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

echo "Input files: ${INPUT[@]}"
echo "Output file: $OUTPUT"

IFS=","
counter=0
for val in "${INPUT[@]}"; do
    #skip the first entry in ${INPUT[@]} (however it is empty)
    if (($(bc <<< "$counter == 0"))); then
      echo "The first entry in the array is skipped, it is empty!"
      counter=$((counter+1))
      continue
    fi

    echo " - $val"

    [ ! -f $val ] && { echo "$val file not found"; exit 99; }
    while read vdo msb shot id start end duration fps
    do
      echo "$id,$start,$end,$shot" >> $OUTPUT
      echo "Counter: $counter"
      counter=$((counter+1))
    done < $val
done

echo "Sum of shots: $counter"
