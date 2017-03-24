#!/bin/bash

#  #######################################################
#  extract_annos (Version 1.0) ###########################

#  ******_****_******_*******
#  *****| |**| |****| |******
#  **___| | _| | ___| | ____
#  */ __| |/ / |/ _ \ __|_  /
#  *\__ \   <| |  __/ |_ / /
#  *|___/_|\_\_|\___|\__/___|
#  **************************
#  **************************

#  ########################################################
#  
#  ########################################################

INPUT=$1
OUTPUT=$2

	echo Input Directory: $1
	echo Output Directory: $2

    for dir in $1/*
    do
        #echo directory: $dir
        #echo parent dir: $(dirname "$dir")
        ##echo parent only: "${dir##*/}"
        
        for file in $dir/*
        do
            #echo file: $file
            #echo filename: $(basename "$file")
            
            name=$(echo $(basename "$file") | sed -e 's/\.[^.]*$//')
            #echo $name
            
            mkdir -p $OUTPUT${dir##*/}
            eval "$(ffprobe -v error -of flat=s=_ -show_entries format=duration $file)"
            ffmpeg -i $file -ss "$(echo "$format_duration/2" | bc)" -q:v 2 -frames:v 1 $OUTPUT${dir##*/}/$name.jpg

        done
        
    done
