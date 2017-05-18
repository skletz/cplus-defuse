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
    -s    Directory containig shot boundaries Csv-File: <start> <endframe>
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
FILENAME=$(basename "$VDO")

DIR=$(dirname "$VDO")
OUTPUTDIR=$(dirname "$DIR")/shots
CSV=$CSV/${FILENAME%.*}.csv
echo "Video File: $VDO; CSV File: $CSV"

mkdir -p "$OUTPUTDIR"
touch $(dirname "$DIR")/videos-less-1sec.csv
touch $(dirname "$DIR")/videos-shots.csv

INPUT=$CSV
OLDIFS=$IFS
FPS=$(avprobe -v error -show_format -show_streams ${VDO} | grep avg_frame_rate | head -n1 | cut -d '=' -f 2)
FPS=$(echo "scale=2; ${FPS}" | bc -l)
WIDTH=$(avprobe -v error -show_format -show_streams ${VDO} | grep width | cut -d '=' -f 2)
HEIGHT=$(avprobe -v error -show_format -show_streams ${VDO} | grep height | cut -d '=' -f 2)
echo "FPS: $FPS; WIDTH: $WIDTH; HEIGHT: $HEIGHT"
#BITRATE=$(avprobe -v error -show_format -show_streams ${VDO} | grep -m2 bit_rate | tail -n1 | cut -d '=' -f 2)

counter=0
IFS=","
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read start end
do
  counter=$((counter+1))

  #echo "Start: $start; End: $end"
  SECOND_START=$(echo "$start / $FPS" | bc -l);
  #echo "Second_Start: $SECOND_START; FPS: $FPS"
  SECOND_END=$(echo $end / $FPS | bc -l);
  TIMESTAMP=$SECOND_START
  #TIMESTAMP=$(date -d@$SECOND_START -u +%H:%M:%S.%s)

  DURATION=$(echo $SECOND_END - $SECOND_START | bc -l);
  #echo "test"
  if (($(bc <<< "$DURATION < 1")))
  then
    echo "===>Less than 1 Second"
    DURATION=0$(echo "$DURATION")
    #VIDEO ID, SHOT ID = start frame number, end frame number
    echo "$VDO,$CSV,${FILENAME%.*},$start,$end,$DURATION,$FPS" >> $(dirname "$DIR")/videos-less-1sec.csv
    continue
  fi

  if (($(bc <<< "$TIMESTAMP < 1")))
  then
    TIMESTAMP=0$(echo "$TIMESTAMP")
  fi

  OUTPUT=${FILENAME%.*}"_"$counter"_"$start-$end"_"$FPS"_"$WIDTH"x"$HEIGHT.mp4
  echo "$VDO,$CSV,$OUTPUT,${FILENAME%.*},$start,$end,$DURATION,$FPS" >> $(dirname "$DIR")/videos-shots.csv

  #DURATION=$(date -d@$DURATION -u +%H:%M:%S.%s)

  echo "TIMESTAMP: $TIMESTAMP; SECOND: $SECOND_START"
  echo "DURATION: $DURATION; SECOND: $SECOND_END; TMP: $(echo $SECOND_END - $SECOND_START | bc -l);"
	echo "Shot: $start - $end"

  echo "Output: $DIR/$OUTPUT"
  echo "avconv -i $VDO -ss $TIMESTAMP -t $DURATION -c:v libx264 -an -sn $OUTPUTDIR/$OUTPUT"
  avconv -y -i $VDO -ss $TIMESTAMP -t $DURATION -c:v libx264 -an -sn $OUTPUTDIR/$OUTPUT


done < $INPUT
IFS=$OLDIFS
