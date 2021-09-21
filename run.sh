#!/bin/bash
echo "Compiling code openCV1 ... "
g++ -pg `pkg-config opencv --cflags` Sabre_opencv1.cpp -o Debug_Sabre_opencv1 `pkg-config opencv --libs`
video_input=0
count=3
    echo "Running the Application v$i"
for i in $(seq $count); do
    ./Debug_Sabre_opencv1 $video_input $i 

    echo "Profiling ..."
    hash=$(date +%s)
    gprof OJ_Projet > prof/resultat_profiling_${hash}_${i}.txt
    echo "Profiling file -> prof/resultat_profiling_opencv1_${hash}_${i}.txt"
    sleep 3
done



echo "Compiling code openCV3 ... "
g++ -pg `pkg-config opencv --cflags` Sabre_opencv3.cpp -o Debug_Sabre_opencv3  `pkg-config opencv --libs`

echo "Running the Application"

./Debug_Sabre_opencv3 0

echo "Profiling ..."
hash=$(date +%s)
gprof Debug_Sabre_opencv3 > prof/resultat_profiling_$hash.txt

echo "Profiling file -> prof/resultat_profiling_$hash.txt"