# Problem setup
<img src="https://github.com/user-attachments/assets/0f7b56fc-4fcd-43af-862b-03ee060e48f8" width="40%" height="40%">   
<img src="https://github.com/user-attachments/assets/5f76de98-3d9e-474a-ba30-36be5c160f08" width="40%" height="40%">  

This benchmark is a plane strain quasi-static problem involving a rectangular block with downward
traction applied in the center of the top side of the mesh and zero vertical displacements applied on the
bottom boundary.   The structure uses a modified Neo-Hookean material model.

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
 <tr>
    <td width="50%">$IB_{4}$   ($\nu = -1$)</td>
    <td width="50%">$CBS_{32}$ ($\nu = -1$)</td>
  </tr>
  <tr>
    <td width="50%"><img src="https://github.com/user-attachments/assets/8a4b20b9-a0ab-4c42-9fda-99c4c73698b7" width=100% height=100%> </td>
    <td width="50%"><img src="https://github.com/user-attachments/assets/3b9dc327-4f87-4fdc-b244-0df930ed4885" width=100% height=100%> </td>
  </tr>
</table>
 
