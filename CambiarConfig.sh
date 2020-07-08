#!/bin/bash
moverCarpeta(){
var="$1"
echo ···········································
echo '  Moviendo Carpeta configs'$var' al configs  '
echo ...........................................
rm -r configs
cp -r ConfigsPruebas/configs$var .
mv configs$var configs
echo
echo Presiona Cualquier tecla Rey ..
read var2
clear
}
moverCarpeta $1
