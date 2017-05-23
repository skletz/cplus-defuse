#!/bin/bash

# A script to find missing shots based on extracted shot list and a directory containing the shots
# @author skletz
# @version 1.0 18/05/07

SHOTS=""
GT=""
OUTPUT=""

USAGE="A tool for dividing a video into shots.
Usage: `basename $0` [-s] [-g]
    -h    Shows this help
    -s    Shots
    -g    Ground Truth
    -o    Output File
 Examples:
    `basename $0` -s shots.csv -g gt.csv -o subset.csv"

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts s:g:o:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    s)  SHOTS=$OPTARG ;;
    g)  GT=$OPTARG ;;
    o)  OUTPUT=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

echo "Video File: $VDO; CSV File: $CSV"
GT_FILENAME=$(basename "$GT")

NEW_GT=$(dirname "$GT")/${GT_FILENAME%.*}_l1s.csv


echo "Shots: $SHOTS; GT: $GT"
echo "Output file: $OUTPUT"
echo "New Ground Truth File: $NEW_GT"

IFS=","
countshots=0
countmatches=0
[ ! -f $SHOTS ] && { echo "$SHOTS file not found"; exit 99; }
[ ! -f $GT ] && { echo "$GT file not found"; exit 99; }
while read vdo start end
do
  echo "Counter: $countshots"
  countshots=$((countshots+1))
  
  while read qid vid sid
  do
    #echo "$vdo == $vid"
    if (($(bc <<< "$vdo == $vid && $sid == $start"))); then
        echo "VDO: $vdo, VID: $vid, SID: $sid, Start: $start => Matched Shot"
        echo "$qid,$vid,$sid" >> $OUTPUT
        countmatches=$((countmatches+1))
    fi

  done < $GT


done < $SHOTS


echo "Sum of shots: $countshots"
