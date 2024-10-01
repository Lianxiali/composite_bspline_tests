#!/bin/bash

MAIN_DIR=$PWD
OUTPUT_DIR=/pine/scr/j/a/jaeholee/IBFE_benchmark/Elastic-Band
NVALS="128"
MFACS="0.5 0.75 1.0 2.0 4.0"

for nval in $NVALS; do
	mkdir ${OUTPUT_DIR}/N=${nval}
	cd ${OUTPUT_DIR}/N=${nval}
for kernel in BSPLINE_3; do
for mfac in $MFACS; do
    	mkdir ${OUTPUT_DIR}/N=${nval}/${kernel}_MFAC_${mfac}
   	cd ${OUTPUT_DIR}/N=${nval}/${kernel}_MFAC_${mfac}
   	
	cp ${MAIN_DIR}/input2d.ss .
        cp ${MAIN_DIR}/run.sh .
        ln -s ${MAIN_DIR}/main .
	
	perl -pi -e "s/N_NUM/$nval/" input2d.ss
	perl -pi -e "s/MFAC_NUM/$mfac/" input2d.ss
	perl -pi -e "s/KERN_FCN/$kernel/" input2d.ss
	perl -pi -e "s/N_NUM/$nval/" run.sh

	sbatch run.sh
done
done
done
