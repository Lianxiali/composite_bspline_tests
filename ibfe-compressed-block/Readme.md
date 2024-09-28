

# Problem setup
<img src="https://github.com/user-attachments/assets/0f7b56fc-4fcd-43af-862b-03ee060e48f8" width="40%" height="40%">   
<img src="https://github.com/user-attachments/assets/5f76de98-3d9e-474a-ba30-36be5c160f08" width="40%" height="40%">  

This benchmark is a plane strain quasi-static problem involving a rectangular block with downward
traction applied in the center of the top side of the mesh and zero vertical displacements applied on the
bottom boundary.   The structure uses a modified Neo-Hookean material model.

***

# Fine mesh (N = 32), Modified Invariants

# Ref
- D. Wells, B. Vadala-Roth, Jae H. Lee, Boyce E. Griffith,  [A Nodal Immersed Finite Element-Finite Difference Method](https://arxiv.org/abs/2111.09958), Journal of computational physics 477 (2023): 111890.


# $IB_4$ Nodal coupling ($\nu = 0.4$)
<p align="center">
<img src="https://github.com/user-attachments/assets/32202962-af34-465a-90ec-dddf9ce39afb" width="40%" height="40%">   
<img src="https://github.com/user-attachments/assets/5ef44660-e3a5-4676-97d4-e1fdb8a739cd" width="40%" height="40%"> 
</p>



<p align="center">
t = 100 s  
</p>

| Density | $\mu$ (fluid)| M  	| MFAC|   N	|Kernel|  DT	  |SHEAR_MOD |$\nu$ (Poisson)| BULK_MOD| KAPPA | ELEM_TYPE|
|--------	|--------	     |----	|----	|-----|------|--------|----------|-----          |---------| ----- |----      |
| 1       |  0.16  	     | 32 	| 0.5 | 32	| IB_4 | 0.00125|80.194    |0.4           | 374.239 | 2500  |QUAD4     |


# $CBS_{32}$ Nodal coupling ($\nu = 0.4$)
Everything is the same except for the kernel function.
<p align="center">
<img src="https://github.com/user-attachments/assets/7e771050-1dd7-4e0f-b9bb-444331da387b" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/fb4b73eb-7d8d-475a-b994-fb8f68cd281e" width="40%" height="40%"> 
</p>
<p align="center"> t = 20s </p>

<p align="center"> 
<img src="https://github.com/user-attachments/assets/5df7bfa5-fe98-4a44-963f-7896a1e8ddba" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/111072be-9559-49ac-9482-e3ce09dda1b5" width="40%" height="40%"> 
</p>
<p align="center"> t = 100s </p>

# $CBS_{32}$ Nodal coupling ($\nu = -1$)
<p align="center">
<img src="https://github.com/user-attachments/assets/6a26da2a-7eb9-4ea1-8cc9-6fb59af3d0ce" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/9c52a710-18e1-4f50-b8e4-e672311b886e" width="40%" height="40%"> 
</p>
<p align="center"> t = 20s </p>

<p align="center">
<img src="https://github.com/user-attachments/assets/9c3f201a-44a1-4622-b625-4fc52abe907f" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/9e961099-73d3-4aec-9f54-265d2475c158" width="40%" height="40%"> 
</p>
<p align="center"> t = 100s </p>

# $IB_{4}$ Nodal coupling ($\nu = -1$)
<p align="center">
<img src="https://github.com/user-attachments/assets/c159433d-0075-408e-b623-75fce0b1a8ee" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/141e4146-1e58-4c1c-830d-918585c62546" width="40%" height="40%"> 
</p>
<p align="center"> t = 20s </p>
<p align="center">
<img src="https://github.com/user-attachments/assets/26b72a02-8de2-45ca-97e1-e2cc16bf0b89" width="40%" height="40%"> 
<img src="https://github.com/user-attachments/assets/85ddae92-8149-424a-85f2-076a111a1d40" width="40%" height="40%"> 
</p>
<p align="center"> t = 100s </p>

# Jacobian rescaled to the same range 

<table>
 <tr align="center">
    <td width="33%">$IB_{4}$   ($\nu = -1$)</td>
    <td width="33%">$CBS_{32}$ ($\nu = -1$)</td>
  <td width="33%">$CBS_{43}$ ($\nu = -1$)</td>
  </tr>
  <tr align="center">
    <td width="33%"><img src="https://github.com/user-attachments/assets/3395acd2-0a52-4c34-9321-1802b0c95a77" width=100% height=100%> </td>   
    <td width="33%"><img src="https://github.com/user-attachments/assets/b1f1f4c4-2df8-4aa9-92e4-dd1aa194f6db" width=100% height=100%> </td>
   <td width="33%"><img src="https://github.com/user-attachments/assets/a63f48ef-bd17-45e6-9913-da2e6c90bd19" width=100% height=100%> </td>
  </tr>

 <tr align="center">
    <td width="33%">$IB_{4}$   ($\nu = 0.4$)</td>
    <td width="33%">$CBS_{32}$ ($\nu = 0.4$)</td>
    <td width="33%">$CBS_{43}$ ($\nu = 0.4$)</td>  
  </tr>
  <tr align="center">
    <td width="33%"><img src="https://github.com/user-attachments/assets/d04c7683-33d3-4b12-a9fc-07c6468dfe9b" width=100% height=100%> </td>   
    <td width="33%"><img src="https://github.com/user-attachments/assets/217d0c75-d247-459b-afd7-229b0b4d4556" width=100% height=100%> </td>
   <td width="33%"><img src="https://github.com/user-attachments/assets/aaefedc2-c7eb-4f15-91cc-a505c458a991" width=100% height=100%> </td>
  </tr> 
  
</table>
 
![image](https://github.com/user-attachments/assets/16bd942c-b04e-4721-b581-34aa838cfd53)


***

<h1> <span style="color:red"> Coarse mesh (N = 16), Modified invariants </span> <h1>

 # Ref
- Ben Vadala-Roth, Shashank Acharya, Neelesh A. Patankar, Simone Rossi, Boyce E. Griffith, [Stabilization approaches for the hyperelastic immersed boundary method for problems of large-deformation incompressible elasticity](https://www.sciencedirect.com/science/article/abs/pii/S0045782520301614?via%3Dihub), Computer Methods in Applied Mechanics and Engineering, 2020(365): 112978.

<table>
 <tr align="center">
    <td width="33%">$IB_{4}$   ($\nu = -1$)</td>
    <td width="33%">$CBS_{32}$ ($\nu = -1$)</td>
  <td width="33%">$CBS_{43}$ ($\nu = -1$)</td>
  </tr>
  <tr align="center">
    <td width="33%"><img src="https://github.com/user-attachments/assets/07d9f642-0d9e-4248-a70f-a3304df5e88c" width=100% height=100%> </td>   
    <td width="33%"><img src="https://github.com/user-attachments/assets/970ef4f0-fb49-4230-bd32-11686bbf1f84" width=100% height=100%> </td>
    <td width="33%"><img src="https://github.com/user-attachments/assets/28eb9769-3134-4418-b43d-be159e578118" width=100% height=100%> </td>
  </tr>

 <tr align="center">
    <td width="33%">$IB_{4}$   ($\nu = 0.4$)</td>
    <td width="33%">$CBS_{32}$ ($\nu = 0.4$)</td>
    <td width="33%">$CBS_{43}$ ($\nu = 0.4$)</td>  
  </tr>
  <tr align="center">
    <td width="33%"><img src="https://github.com/user-attachments/assets/274cb8f7-35f4-4bde-b659-227c61611a5a" width=100% height=100%> </td>   
    <td width="33%"><img src="https://github.com/user-attachments/assets/fc995bb2-fd7b-41bb-81f9-546c4a3e7bc3" width=100% height=100%> </td>
   <td width="33%"><img src="https://github.com/user-attachments/assets/be008e0e-c8c7-48dc-8b75-e8e7aeb2d8a8" width=100% height=100%> </td>
  </tr> 
  
</table>

![image](https://github.com/user-attachments/assets/73b5b33c-eee7-4680-851a-ddbe14ba1c7e)



***

# Coarse mesh (N = 16)
## Reproducing the bad results in Ben's paper

The following results are with nodal coupling, MFAC = 1, endtime = 500s, loadtime = 100s.

<table>
 <tr align="center">
    <td width="50%">$IB_{4}$   ($\nu = -1$, MFAC = 1, QUAD4, Elemental coupling, t = 100s)</td>
    <td width="50%">$CBS_{32}$ ($\nu = -1$, MFAC = 1, QUAD4, Elemental coupling, t = 96s ()</td>
  </tr>
  <tr align="center">
    <td width="50%"><img src="https://github.com/user-attachments/assets/04a7b1ee-d6af-49e2-b21a-8cad09be3ece" width=100% height=100%> </td>   
    <td width="50%"><img src="https://github.com/user-attachments/assets/8e98c60a-7f17-4935-ad64-9e33fd38e378" width=100% height=100%> </td>
  </tr>
  <tr align="center">
    <td width="50%">This reproduced the bad results in Ben's paper</td>
    <td width="50%">$CBS_{32}$ does not work with MFAC = 1</td>
  </tr>
</table>

## Comparison between Unmodified and Modified invariants 

The following results are at t = 100s, with nodal coupling, MFAC = 0.5, $\nu = -1$, END_TIME = 100s, and LOAD_TIE = 40s.

<table>
 <tr align="center">
    <td width="10%"> Invariants</td>  
    <td width="30%">$IB_{4}$   (QUAD4)</td>
    <td width="30%">$CBS_{32}$ (QUAD4)</td>
    <td width="30%">$CBS_{32}$ (TRI3)</td>
  </tr>  
  <tr align="center">
   <td width="10%"> Unmodified</td>
    <td width="30%"><img src="https://github.com/user-attachments/assets/e626742d-1379-4b6e-8b5c-b2d44d6b6530" width=100% height=100%> </td>
    <td width="30%"><img src="https://github.com/user-attachments/assets/e2729166-d54c-45bd-a9ac-aadb5ebf3041" width=100% height=100%> </td>
    <td width="30%"><img src="https://github.com/user-attachments/assets/0981dfd4-04d7-45c6-a258-ce6e3ed9b797" width=100% height=100%> </td>
  
  <tr align="center">
    <td width="10%"> Modified</td>   
    <td width="30%"><img src="https://github.com/user-attachments/assets/5381c35a-1d78-4870-a324-919da1d2fe94" width=100% height=100%> </td>   
    <td width="30%"><img src="https://github.com/user-attachments/assets/b7e8a69a-ca31-4811-9b66-b20b0a7969fd" width=100% height=100%> </td>
    <td width="30%"><img src="https://github.com/user-attachments/assets/91f07d16-acd6-481d-a22c-fb14d9cade34" width=100% height=100%> </td>
  </tr>
  <tr align="center">
    <td width="10%"> Remarks</td>   
    <td width="30%">Improved a lot</td>
    <td width="30%">Improved a little</td>
    <td width="30%">Improved a little</td>
  </tr>
</table>





