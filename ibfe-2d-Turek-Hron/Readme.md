# ISSUE in the original case from fe_benchmarks/2d-Turek-Hron
[1] undefined in input2d.tune:  
    1. N_NUM  
    2. DUMP_INTRVL  
    3. ERROR: Unrecognized file extension: data/BEAM_FILE  
        This mesh file does not exist.  

[2] main.cpp  
    [-] need to comment out 
        // #include <IBAMR_config.h>  
        // #include <IBTK_config.h>  
    [-] warnings (deprecated functions)  
        - IBFEMethod::VELOCITY_SYSTEM_NAME  
        - IBFEMethod::FORCE_SYSTEM_NAME  
        - IBFEMethod::COORDS_SYSTEM_NAME  
        
**Note: the source file _example.cpp_ and _input2d_ are actually from examples/IBFE/explicit/ex6**  
The input file has an issue:
```
DX0 = H/N                           
DX  = H/NFINEST  
```
*H* was wrongly set as *L*, which causes the solid mesh to be very coarse.

![image](https://github.com/user-attachments/assets/f15c4882-f95d-45a5-aeab-0b3b53993655)

Parameters for the Turek-Hron benchmark

| Density                                        	| ρ  	| 1,000  	| kg 	|
|------------------------------------------------	|----	|--------	|----	|
|Viscosity| µ| 1.0| Pa · s|
|Material model| -| modified neo-Hookean| -|
|Shear modulus| G| 2.0| MPa|
|Numerical bulk modulus| κstab| 18.78 − 75.12| MPa|
|Final time| Tf |12.0| s|
|Load time| Tl |2.0| s|
