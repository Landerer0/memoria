#!/bin/bash
#chmod +x estimaciontopk.sh

# caso MRL
#./estimaciontopk.sh nombreArchivo.txt k mink 

#./estimaciontopk.sh Mawi-20161201.txt 20 200 
#./estimaciontopk.sh Mawi-20171101.txt 20 200 
#./estimaciontopk.sh Mawi-20181201.txt 20 200 
#./estimaciontopk.sh Mawi-20191102.txt 20 200 
#./estimaciontopk.sh Mawi-20200901.txt 20 200
#./estimaciontopk.sh Chicago-20080319.txt 20 200 
#./estimaciontopk.sh Chicago-20080515.txt 20 200 
#./estimaciontopk.sh Chicago-20110608.txt 20 200 
#./estimaciontopk.sh Chicago-20150219.txt 20 200 
#./estimaciontopk.sh Chicago-20160121.txt 20 200 
#./estimaciontopk.sh Mendeley.txt 20 200 
#./estimaciontopk.sh Sanjose-20081016.txt 20 200 


# caso KLL, ¡ES IMPORTANTE QUE LOS VALORES FLOTANTES SE ENTREGUEN 6 DECIMALES!
#./estimaciontopk.sh nombreArchivo.txt k epsilon delta c mink

#./estimaciontopk.sh Mawi-20161201.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Mawi-20171101.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Mawi-20181201.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Mawi-20191102.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Mawi-20200901.txt 20 0.005000 0.001000 0.666666 20
#./estimaciontopk.sh Chicago-20080319.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Chicago-20080515.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Chicago-20110608.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Chicago-20150219.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Chicago-20160121.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Mendeley.txt 20 0.005000 0.001000 0.666666 20 
#./estimaciontopk.sh Sanjose-20081016.txt 20 0.005000 0.001000 0.666666 20 


# Nombre de ejecutables que se ejecutaran
kllsave="./kllSave"
topkerrorbin="./topkerrorbin"

if [ $# -eq 3 ]; then #caso mrl
    archivoTraza=$1
    k=$2
    mink=$3

    nombreArchivoTraza=${archivoTraza::-4} # quitar .txt
    nombreBinario=$nombreArchivoTraza.mrlk$mink.bin

    eval $kllsave "$archivoTraza" "$mink"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
fi

if [ $# -eq 6 ]; then #caso kll
    archivoTraza=$1
    k=$2
    epsilon=$3
    delta=$4
    c=$5
    mink=$6

    nombreArchivoTraza=${archivoTraza::-4} # quitar .txt
    nombreBinario="$nombreArchivoTraza"".klle""$epsilon"d"$delta"c$c"mk"$mink.bin

    eval $kllsave $archivoTraza $epsilon $delta $c $mink
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
fi


