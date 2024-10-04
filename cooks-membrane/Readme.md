| Density | $\mu$  	| M  	| MFAC|   N	|Kernel|  DT	     |SHEAR_MOD | BULK_MOD|POISSON_RATION| KAPPA       | ELEM_TYPE| Coupling|
|--------	|--------	|----	|----	|-----|------|--------   |----------|---------|--------------| -----       |----      |---------|
| 1       |  0.16  	| 32 	| 2   | 90	| **** | 0.00010101|83.3333   | 833306  |0.49995       |1.2375e+06   |QUAD9     |Elemental|

<img src="https://github.com/user-attachments/assets/7efd39c5-7ef5-45e0-ac33-8fdcdfc581ef" width="50%" height="50%">  
Shear locking with P1 (TRI3) element

<img src="https://github.com/user-attachments/assets/08558a92-38d2-4fe3-9d72-c86fbb17b849" width="40%" height="40%"> <img src="https://github.com/user-attachments/assets/dadb503f-1e0c-4760-a4f2-88c8bb99644d" width="40%" height="40%">  
Elemental coupling with P2 (QUAD9) element (32X32)  Nodal coupling with P2(QUAD9) element (32X32)  No difference from Elemental coupling


![image](https://github.com/user-attachments/assets/cae4dec9-6173-4dc3-8abc-e5f30122dde2)  
Displacement history at the top-right corner  

![image](https://github.com/user-attachments/assets/a67bcb35-ea4d-4242-9ed0-16d59326a2b8)  
Zoomed in

# Volume conservation  

## conventional kernels  

<img src="https://github.com/user-attachments/assets/8f3a3590-00b3-4bfb-90b0-f2e020887d23" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/22a70a4b-02eb-4d26-84b1-0452f12f290f" width="40%" height="40%">

<img src="https://github.com/user-attachments/assets/699c2fe6-6e3a-4deb-9679-d2593a077b71" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/05d71a8b-eb43-4b8d-aaca-7223ebde3ef8" width="40%" height="40%">

## Isotropic B-spline kernels  

<img src="https://github.com/user-attachments/assets/b78dbde7-ce3a-498c-b0d6-3df8a73dcb15" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/cbcfb184-5627-43e0-9db8-ca3be161a826" width="40%" height="40%">

<img src="https://github.com/user-attachments/assets/0d9b3cd3-2c36-44b0-9d02-923caa02b9f4" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/7e6a91bb-a5da-46ed-91d4-bca18b8058c3" width="40%" height="40%">

<img src="https://github.com/user-attachments/assets/07c7ada2-177f-4a99-a73b-0f597d9eeb48" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/9d567480-e4da-4861-bbdf-5dba96b87807" width="40%" height="40%">

## Composite B-spline kernels

<img src="https://github.com/user-attachments/assets/51145681-a165-4a3e-983d-6bdf33549a47" width="40%" height="40%">
<img src="https://github.com/user-attachments/assets/2f6db1b0-0689-4b4a-842c-4d6c8067e7e0" width="40%" height="40%">

Table: Fluid grid number N = MFAC*M*lx/LX for grid convergence test (MFAC fixed, fluid and solid refine simultaneously)  
| M/MFAC 	| 0.5 	| 0.75 	|  1  	| 1.25 	| 1.5 	|
|:------:	|:---:	|:----:	|:---:	|:----:	|:---:	|
|    4   	|  4  	|   6  	|  8  	|  10  	|  12 	|
|    8   	|  8  	|  12  	|  16 	|  20  	|  24 	|
|   16   	|  16 	|  24  	|  32 	|  40  	|  48 	|
|   32   	|  32 	|  48  	|  64 	|  80  	|  96 	|
|   48   	|  48 	|  72  	|  96 	|  120 	| 144 	|
|   64   	|  64 	|  96  	| 128 	|  160 	| 192 	|

Note: lx = fluid domain length = 13, LX = solid domain length = 6.5  
Note: I expand the fluid domain sizes from 10x10 to 13x13 to make the grid refinement convenient (to ensure the grid number an integer)  


