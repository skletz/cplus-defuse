#!/bin/bash

# A script for dividing the videos of the TREVid dataset into the proposed master-shots
# Requirements: avprobe, avconv
# @author skletz
# @version 1.1 23/05/07
# @making 15/05/07

VDO_FILE=""
MSB_DIR=""
P_ID=""

USAGE="A script for dividing the videos of the TREVid dataset into the proposed master-shots avprobe, avconv
The output contains two further files: videos-msb-l1s.csv (shots that are less than 1 second), videos-msb.csv (all extracted shots)
Usage: `basename $0` [-video] [-MSB]
    -h    Shows help
    -v    Video-File <.MP4>
    -s    VDO_DIRectory that contain the master shot boundaries <Video-File.MSB>: <startframe>,<endframe>
    -p    ID <p1,p2 ...> for the identificaiton of the output files
 Examples:
    bash `basename $0` -v ../data/trecvid/videos/35345.mp4 -s ../data/trecvid/msbs/ -p p1"

# parse command line
if [ $# -eq 0 ]; then #  must be at least one arg
    echo "$USAGE" >&2
    exit 1
fi

while getopts v:s:p:h OPT; do
    case $OPT in
    h)  echo "$USAGE"
        exit 0 ;;
    v)  VDO_FILE=$OPTARG ;;
    s)  MSB_DIR=$OPTARG ;;
    p)  P_ID=$OPTARG ;;
    \?) # getopts issues an error message
        echo "$USAGE" >&2
        exit 1 ;;
    esac
done
shift `expr $OPTIND - 1`

# show input
echo "Video-File: $VDO_FILE; VDO_DIRectory of Master-Shot-Boundaries: $MSB_DIR"

# prepare output
VDO_FILENAME=$(basename "$VDO_FILE")
VDO_DIR=$(dirname "$VDO_FILE")
OUTPUTSHOT_DIR=$(dirname "$VDO_DIR")/shots
OUTPUTFILES_DIR=$(dirname "$VDO_DIR")
OUTPUFILE_MSB=$P_ID"_videos-msb.csv"
#less than 1 second
OUTPUFILE_L1S=$P_ID"_videos-msb-l1s.csv"
#less than 5 frames
OUTPUFILE_L5F=$P_ID"_videos-msb-l5f.csv"
MSB_FILE=$MSB_DIR/${VDO_FILENAME%.*}.csv

echo "MSB-File: $MSB_FILE; Outpu-Dir: $OUTPUTSHOT_DIR; Output-Files: $OUTPUTFILES_DIR/$OUTPUFILE_L1S, $OUTPUTFILES_DIR/$OUTPUFILE_MSB"

#rm -rf $OUTPUTSHOT_DIR
#rm -rf $OUTPUTFILES_DIR/$OUTPUFILE_L1S
#rm -rf $OUTPUTFILES_DIR/$OUTPUFILE_L5F
#rm -rf $OUTPUTFILES_DIR/$OUTPUFILE_MSB
mkdir -p "$OUTPUTSHOT_DIR"
touch $OUTPUTFILES_DIR/$OUTPUFILE_L1S
touch $OUTPUTFILES_DIR/$OUTPUFILE_L5F
touch $OUTPUTFILES_DIR/$OUTPUFILE_MSB

# get video information
FPS=$(avprobe -v error -show_format -show_streams ${VDO_FILE} | grep avg_frame_rate | head -n1 | cut -d '=' -f 2)
FPS=$(echo "scale=2; ${FPS}" | bc -l)
WIDTH=$(avprobe -v error -show_format -show_streams ${VDO_FILE} | grep width | cut -d '=' -f 2)
HEIGHT=$(avprobe -v error -show_format -show_streams ${VDO_FILE} | grep height | cut -d '=' -f 2)
echo "Video: $VDO_FILE; FPS: $FPS; WIDTH: $WIDTH; HEIGHT: $HEIGHT"
#BITRATE=$(avprobe -v error -show_format -show_streams ${VDO_FILE} | grep -m2 bit_rate | tail -n1 | cut -d '=' -f 2)

#input stream
INPUT=$MSB_FILE
OLDIFS=$IFS

#line number is the shot id in msb files
shotid=0

#iterate throug csv file (msb file)
IFS=","
[ ! -f $INPUT ] && { echo "$INPUT file not found"; exit 99; }
while read start end
do
  #shot id starts with line 1
  shotid=$((shotid+1))
  SECOND_START=$(echo "$start / $FPS" | bc -l);
  SECOND_END=$(echo $end / $FPS | bc -l);
  TIMESTAMP=$SECOND_START
  echo "Start: $start; End: $end"
  #TIMESTAMP=$(date -d@$SECOND_START -u +%H:%M:%S.%s)

  DURATION=$(echo $SECOND_END - $SECOND_START | bc -l);
  #frames per segment
  FRAMES_SEGMENT=$(echo "$end - $start" | bc -l);
  echo "Frames per Segment: $FRAMES_SEGMENT"

  if (($(bc <<< "$DURATION < 1")))
  then
    echo "===> Shot has less than 1 second (duration)"
    #ffmpeg, avprobe invalid input if the leading zero is missing
    DURATION=0$(echo "$DURATION")
    #VIDEO ID, SHOT ID = start frame number, end frame number
    echo "$VDO_FILE,$MSB_FILE,${VDO_FILENAME%.*},$shotid,$start,$end,$FRAMES_SEGMENT,$DURATION,$FPS" >> $OUTPUTFILES_DIR/$OUTPUFILE_L1S
    #continue
  fi

  if (($(bc <<< "$FRAMES_SEGMENT < 5")))
  then
    echo "===> Shot has less than 5 frames per segment"
    #VIDEO ID, SHOT ID = start frame number, end frame number
    echo "$VDO_FILE,$MSB_FILE,${VDO_FILENAME%.*},$shotid,$start,$end,$FRAMES_SEGMENT,$DURATION,$FPS" >> $OUTPUTFILES_DIR/$OUTPUFILE_L5F    #continue
  fi

  if (($(bc <<< "$TIMESTAMP < 1")))
  then
    TIMESTAMP=0$(echo "$TIMESTAMP")
  fi

  OUTPUT=${VDO_FILENAME%.*}"_"$shotid"_"$start-$end"_"$FPS"_"$WIDTH"x"$HEIGHT.mp4
  echo "$VDO_FILE,$MSB_FILE,${VDO_FILENAME%.*},$shotid,$start,$end,$FPSEGMENT,$DURATION,$FPS" >> $OUTPUTFILES_DIR/$OUTPUFILE_MSB

  #DURATION=$(date -d@$DURATION -u +%H:%M:%S.%s)

  echo "TIMESTAMP: $TIMESTAMP; SECOND: $SECOND_START"
  echo "DURATION: $DURATION; SECOND: $SECOND_END; TMP: $(echo $SECOND_END - $SECOND_START | bc -l);"
	echo "Shot: $start - $end"

  echo "Output: $VDO_DIR/$OUTPUT"
  echo "avconv -i $VDO_FILE -ss $TIMESTAMP -t $DURATION -c:v libx264 -an -sn $OUTPUTSHOT_DIR/$OUTPUT"
  avconv -y -i $VDO_FILE -ss $TIMESTAMP -t $DURATION -c:v libx264 -an -sn $OUTPUTSHOT_DIR/$OUTPUT


done < $INPUT
IFS=$OLDIFS
