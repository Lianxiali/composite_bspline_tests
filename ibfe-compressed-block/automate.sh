 #!/bin/bash

echo "PID IS $$"

for ELEM in TRI3 QUAD4 TRI6 QUAD9; do
  mkdir elem=$ELEM
  cd elem=$ELEM
  for NU in -1.0 0.0 .4 .49995; do
    mkdir nu=$NU
    cd nu=$NU
    for M in 4 8 16 32 64; do

      echo $PWD
      cp ../../input2d input2d_$M

      perl -pi -e "s/ELEM_TEMP/$ELEM/" ./input2d_$M
      perl -pi -e "s/NU_TEMP/$NU/" ./input2d_$M
      perl -pi -e "s/M_TEMP/$M/" ./input2d_$M
      
      perl -pi -e "s/IB2d.log/IB2d_$M.log/" ./input2d_$M
      perl -pi -e "s/viz_IB2d/viz_IB2d_$M/" ./input2d_$M
      perl -pi -e "s/restart_IB2d/restart_IB2d_$M/" ./input2d_$M
      perl -pi -e "s/hier_data_IB2d/hier_data_IB2d_$M/" ./input2d_$M
      
      if [ $ELEM = TRI3 ]
      then
	perl -pi -e "s/PK1_DEV_QUAD_ORDER_TEMP/FIRST/" ./input2d_$M
	perl -pi -e "s/PK1_DIL_QUAD_ORDER_TEMP/FIRST/" ./input2d_$M
	
	perl -pi -e "s/MFAC_TEMP/1/" ./input2d_$M
	
      elif [ $ELEM = QUAD4 ]
      then
	perl -pi -e "s/PK1_DEV_QUAD_ORDER_TEMP/SECOND/" ./input2d_$M
	perl -pi -e "s/PK1_DIL_QUAD_ORDER_TEMP/SECOND/" ./input2d_$M
	
	perl -pi -e "s/MFAC_TEMP/2/" ./input2d_$M
      elif [ $ELEM = TRI6 ]
      then
	perl -pi -e "s/PK1_DEV_QUAD_ORDER_TEMP/FOURTH/" ./input2d_$M
	perl -pi -e "s/PK1_DIL_QUAD_ORDER_TEMP/FOURTH/" ./input2d_$M
	
	perl -pi -e "s/MFAC_TEMP/1/" ./input2d_$M
      elif [ $ELEM = QUAD9 ]
      then
	perl -pi -e "s/PK1_DEV_QUAD_ORDER_TEMP/FIFTH/" ./input2d_$M
	perl -pi -e "s/PK1_DIL_QUAD_ORDER_TEMP/FIFTH/" ./input2d_$M
	
	perl -pi -e "s/MFAC_TEMP/2/" ./input2d_$M
      fi

    done
    ln -s ../../main .
    cp ../../run_IBFE_comparisons.sh run_IBFE_comparisons.sh
    chmod +x run_IBFE_comparisons.sh
    bsub -q week -n 1 -J ${ELEM}_NU=${NU} ./run_IBFE_comparisons.sh
    
    cd ..
  done
  cd ..
done
