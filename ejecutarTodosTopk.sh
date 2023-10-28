#!/bin/bash
#chmod +x ejecutarTodosTopk.sh

#./ejecutarTodosTopk.sh k epsilon delta c mink mrlk
#./ejecutarTodosTopk.sh 20 0.005000 0.001000 0.666666 20 200 
# nohup ./ejecutarTodosTopk.sh 20 0.005000 0.001000 0.666666 20 200 &

k=$1
epsilon=$2
delta=$3
c=$4
mink=$5
mrlk=$6

echo se ejecutara fichero bash

eval ./estimaciontopk.sh Mawi-20161201.txt $k $mrlk 
eval ./estimaciontopk.sh Mawi-20171101.txt $k $mrlk 
eval ./estimaciontopk.sh Mawi-20181201.txt $k $mrlk 
eval ./estimaciontopk.sh Mawi-20191102.txt $k $mrlk 
eval ./estimaciontopk.sh Mawi-20200901.txt $k $mrlk
eval ./estimaciontopk.sh Chicago-20080319.txt $k $mrlk 
eval ./estimaciontopk.sh Chicago-20080515.txt $k $mrlk 
eval ./estimaciontopk.sh Chicago-20110608.txt $k $mrlk 
eval ./estimaciontopk.sh Chicago-20150219.txt $k $mrlk 
eval ./estimaciontopk.sh Chicago-20160121.txt $k $mrlk 
eval ./estimaciontopk.sh Mendeley.txt $k $mrlk 
eval ./estimaciontopk.sh Sanjose-20081016.txt $k $mrlk 

eval ./estimaciontopk.sh Mawi-20161201.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Mawi-20171101.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Mawi-20181201.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Mawi-20191102.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Mawi-20200901.txt $k $epsilon $delta $c $mink
eval ./estimaciontopk.sh Chicago-20080319.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Chicago-20080515.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Chicago-20110608.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Chicago-20150219.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Chicago-20160121.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Mendeley.txt $k $epsilon $delta $c $mink 
eval ./estimaciontopk.sh Sanjose-20081016.txt $k $epsilon $delta $c $mink 