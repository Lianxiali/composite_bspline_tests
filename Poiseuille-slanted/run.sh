#!/bin/bash

# Define the text file to be modified
original_file="input2d.ib.pBC_CG"

# Array of values to be assigned to viz_dump_dirname
values=(
    "IB_3"
    "IB_4"
    "DISCONTINUOUS_LINEAR"
    "COMPOSITE_BSPLINE_32"
    "COMPOSITE_BSPLINE_43"
    "COMPOSITE_BSPLINE_54"
    "COMPOSITE_BSPLINE_65"
    "PIECEWISE_LINEAR"
    "BSPLINE_3"
    "BSPLINE_4"
    "BSPLINE_5"
    "BSPLINE_6"
)

# Loop over each value and perform the modification
for value in "${values[@]}"; do
    # Create a copy of the original file for modification
    modified_file="modified_${value}.input"
    cp "$original_file" "$modified_file"
    
    # Use sed to replace the value of viz_dump_dirname in the copied file
    sed -i "s/\(viz_dump_dirname[[:space:]]*=[[:space:]]*\).*/\1\"viz_$value\"/" "$modified_file"

    # Use sed to replace the value of DELTA_FUNCTION in the copied file
    sed -i "s/\(DELTA_FUNCTION[[:space:]]*=[[:space:]]*\).*/\1\"$value\"/" "$modified_file"

    # Use sed to replace the value of DELTA_FUNCTION in the copied file
    sed -i "s/\(log_file_name[[:space:]]*=[[:space:]]*\).*/\1\"log.${value}\"/" "$modified_file"

    # Run the program with the modified file in the background
    ./main2d "$modified_file" -stokes_ksp_rtol 1.0e-12 &

done

# Wait for all background processes to finish
wait
