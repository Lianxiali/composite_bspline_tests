#!/bin/bash
echo "PID IS $$"

MAIN_DIR="$PWD"
OUTPUT_DIR="$PWD/RESULTS"
M_SOLID="4 6 16 32 64 128"
MFACS="0.5 0.75 1.0 2.0 4.0"
KERNELS="IB_3 \
    IB_4 \
    IB_5 \
    IB_6 \
    DISCONTINUOUS_LINEAR \
    COMPOSITE_BSPLINE_32 \
    COMPOSITE_BSPLINE_43 \
    COMPOSITE_BSPLINE_54 \
    COMPOSITE_BSPLINE_65 \
    PIECEWISE_LINEAR \
    BSPLINE_3 \
    BSPLINE_4 \
    BSPLINE_5 \
    BSPLINE_6"

# Loop over solid mesh refinement
for M in $M_SOLID; do
    mkdir -p "${OUTPUT_DIR}/M${M}"
    cd "${OUTPUT_DIR}/M${M}"

    # Loop over the MFACS values
    for mfac in $MFACS; do

        # Create directory for MFAC and move into it
        mkdir -p "MFAC_${mfac}"
        cd "MFAC_${mfac}"

        # Loop over KERNELS
        for kernel in $KERNELS; do
            echo "Processing kernel: $kernel"

            # Create directory for each kernel and move into it
            mkdir -p "${kernel}"
            cd "${kernel}"

            # Copy necessary files
            cp "${MAIN_DIR}/input2d.ss" .
            ln -sf "${MAIN_DIR}/main2d" .

            # Replace placeholders in the input2d.ss file
            perl -pi -e "s/M_TEMP/$M/" input2d.ss 
            perl -pi -e "s/MFAC_TEMP/$mfac/" input2d.ss
            perl -pi -e "s/KERN_FCN/\"${kernel}\"/" input2d.ss

            # Check if the first argument is "parallel"
            if [ "$1" == "parallel" ]; then

                # Create a copy of the original file for modification
                cp "${MAIN_DIR}/sluslurm-longleaf.sh" .

                # Modify job name
                jobname="M${M}_MFAC${mfac}_${kernel}"
                sed -i "s/JOBNAME/$jobname/g" sluslurm-longleaf.sh

                # Submit the modified job file
                sbatch sluslurm-longleaf.sh
                echo "$jobname: job submitted."

            else
                # Execute the main2d program with specified options
                ./main2d input2d.ss -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10 &
            fi

            # Move back to the MFAC directory
            cd ..

        done

        # Move back to the main directory
        cd ..

    done

done
