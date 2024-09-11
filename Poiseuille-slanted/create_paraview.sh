#!/bin/bash

case="viz_IB_3"                                    # Define the base folder name
VizPath="./${case}/visit*/"                        # Data folder path using the variable
OutFileName="./${case}/paraview.samrai.series"     # Output file using the variable
declare -a arrdata=()                              # Declare array for data
declare -a arrfolder=()                            # Declare array for folder names
count=0                                            # Counter for probe data
arrdata+=0                                         # Add a zero in the beginning for probe data.
arrfolder+=($(ls -d $VizPath))                     # Get folder names from VizPath

if test -f "$OutFileName"; then                    # If output file exists, delete it.
    rm "$OutFileName"
fi
arrfolder=( $(printf "%s\n" "${arrfolder[@]}" | sort -V) ) ## Sort folders in 'natural' order
printf "{\n" >> $OutFileName
printf "  \"file-series-version\" : \"1.0\",\n" >> $OutFileName
printf "  \"files\" : [\n" >> $OutFileName

for i in "${arrfolder[@]}"
do
   var=$(echo $i | rev | cut -d'/' -f-2 | rev | sed 's/\//\\\\/g') # Reformat string with folder names
   echo $var
   var2="${var}summary.samrai"                 # Add summary.samrai to folder name strings
   time="${arrdata[$count]}"                   # Get time value
   printf "    { \"name\" : \"${case}\\\\${var2}\", \"time\" : $count }, \n"  >> $OutFileName # Correct Windows path format
   (( count++ ))
done
printf "  ]\n" >> $OutFileName
printf "}" >> $OutFileName
