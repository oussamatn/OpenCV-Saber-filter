#/bash/sh

g++ `pkg-config opencv --cflags` OJ_Sabre.cpp -o OJ_Projet `pkg-config opencv --libs`

./OJ_Projet
