#!/bin/bash

./main input2d_4 -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10
./main input2d_8 -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10
./main input2d_16 -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10
./main input2d_32 -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10
./main input2d_64 -ksp_rtol 1.0e-10 -stokes_ksp_rtol 1.0e-10
