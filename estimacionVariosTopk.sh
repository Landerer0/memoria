#!/bin/bash
#chmod +x estimacionVariosTopk.sh

# caso MRL
#./estimacionVariosTopk.sh nombreArchivo.txt mink 

# nohup ./estimacionVariosTopk.sh Mawi-20161201.txt 200 &
# nohup ./estimacionVariosTopk.sh Mawi-20171101.txt 200 &
# nohup ./estimacionVariosTopk.sh Mawi-20181201.txt 200 &
# nohup ./estimacionVariosTopk.sh Mawi-20191102.txt 200 &
# nohup ./estimacionVariosTopk.sh Mawi-20200901.txt 200 &
# nohup ./estimacionVariosTopk.sh Chicago-20080319.txt 200 &
# nohup ./estimacionVariosTopk.sh Chicago-20080515.txt 200 &
# nohup ./estimacionVariosTopk.sh Chicago-20110608.txt 200 &
# nohup ./estimacionVariosTopk.sh Chicago-20150219.txt 200 &
# nohup ./estimacionVariosTopk.sh Chicago-20160121.txt 200 &
# nohup ./estimacionVariosTopk.sh Mendeley.txt 200 &
# nohup ./estimacionVariosTopk.sh Sanjose-20081016.txt 200 &

# nohup ./estimacionVariosTopk.sh Mawi-20161201.txt 300 &
# nohup ./estimacionVariosTopk.sh Mawi-20171101.txt 300 &
# nohup ./estimacionVariosTopk.sh Mawi-20181201.txt 300 &
# nohup ./estimacionVariosTopk.sh Mawi-20191102.txt 300 &
# nohup ./estimacionVariosTopk.sh Mawi-20200901.txt 300 &
# nohup ./estimacionVariosTopk.sh Chicago-20080319.txt 300 &
# nohup ./estimacionVariosTopk.sh Chicago-20080515.txt 300 &
# nohup ./estimacionVariosTopk.sh Chicago-20110608.txt 300 &
# nohup ./estimacionVariosTopk.sh Chicago-20150219.txt 300 &
# nohup ./estimacionVariosTopk.sh Chicago-20160121.txt 300 &
# nohup ./estimacionVariosTopk.sh Mendeley.txt 300 &
# nohup ./estimacionVariosTopk.sh Sanjose-20081016.txt 300 &


# caso KLL, ¡ES IMPORTANTE QUE LOS VALORES FLOTANTES SE ENTREGUEN 6 DECIMALES!
#./estimacionVariosTopk.sh nombreArchivo.txt k epsilon delta c mink

# nohup ./estimacionVariosTopk.sh Mawi-20161201.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20171101.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20181201.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20191102.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20200901.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20080319.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20080515.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20110608.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20150219.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20160121.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mendeley.txt 0.005000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Sanjose-20081016.txt 0.005000 0.001000 0.666666 20 & 

# nohup ./estimacionVariosTopk.sh Mawi-20161201.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20171101.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20181201.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20191102.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mawi-20200901.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20080319.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20080515.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20110608.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20150219.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Chicago-20160121.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Mendeley.txt 0.001000 0.001000 0.666666 20 &
# nohup ./estimacionVariosTopk.sh Sanjose-20081016.txt 0.001000 0.001000 0.666666 20 & 


# Nombre de ejecutables que se ejecutaran
kllsave="./kllSave"
topkerrorbin="./topkerrorbin"

if [ $# -eq 2 ]; then #caso mrl
    archivoTraza=$1
    mink=$2

    nombreArchivoTraza=${archivoTraza::-4} # quitar .txt
    nombreBinario=$nombreArchivoTraza.mrlk$mink.bin

    eval $kllsave "$archivoTraza" "$mink"
    k="1024"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
    k="2048"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
    k="4096"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
    k="8192"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
    k="16384"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
    k="32768"
    eval $topkerrorbin $archivoTraza $nombreArchivoTraza.mrlk$mink.bin $k  >> resultadosTopk/$nombreBinario.txt
fi

if [ $# -eq 5 ]; then #caso kll
    archivoTraza=$1
    epsilon=$2
    delta=$3
    c=$4
    mink=$5

    nombreArchivoTraza=${archivoTraza::-4} # quitar .txt
    nombreBinario="$nombreArchivoTraza"".klle""$epsilon"d"$delta"c$c"mk"$mink.bin

    eval $kllsave $archivoTraza $epsilon $delta $c $mink

    k="1024"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
    k="2048"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
    k="4096"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
    k="8192"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
    k="16384"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
    k="32768"
    eval $topkerrorbin $archivoTraza $nombreBinario $k >> resultadosTopk/$nombreBinario.txt
fi

