#!/bin/bash
#chmod +x obtenerDatosHistogramas.sh

# NOTA, PARA GRAFICAR HAY QUE AGREGAR EN LA PRIMERA FILA "DISTANCE TRAZA", LA TRAZA SIN ".TXT" 
#./obtenerDatosHistogramas.sh Mendeley.txt
# ["Chicago-20160121", "Mendeley", "Mawi-20181201.txt", "Sanjose-20081016"]
archivo=$1
textoAux="Distance "${archivo::length-4}

#sort -nr $archivo | awk '{printf("%d %d\n", NR-1, $1);}'
#echo $variable > datosHistogramaElementos$1
#echo $variable > datosHistograma$1
sort -nr $archivo  | tr " " "\n" | uniq -c | sort -gru > "datosHistogramaElementos"$1
cat datosHistogramaElementos$1 | awk '{printf("%d %d\n",NR-1,$1);}' > datosHistograma$1