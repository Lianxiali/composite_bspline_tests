# ISSUE in the original case from fe_benchmarks/2d-Turek-Hron
[1] undefined in input2d.tune:  
    1. N_NUM  
    2. DUMP_INTRVL  
    3. ERROR: Unrecognized file extension: data/BEAM_FILE  
        This mesh file does not exist.  

[2] main.cpp  
    [*] need to comment out 
        // #include <IBAMR_config.h>  
        // #include <IBTK_config.h>  
    [*] warnings (deprecated functions)  
        - IBFEMethod::VELOCITY_SYSTEM_NAME  
        - IBFEMethod::FORCE_SYSTEM_NAME  
        - IBFEMethod::COORDS_SYSTEM_NAME  
        
** Note: the source file _example.cpp_ and _input2d_ are actually from examples/IBFE/explicit/ex6**  
The input file has an issue:
```
DX0 = H/N                           
DX  = H/NFINEST  
```
*H* was wrongly set as *L*, which causes the solid mesh is very coarse.
