#Elastic pressurized membrane 

## parameters
A Pressurized Circular Membrane Case
L = 1 (square domain)
R = 0.25 (radius, high pressure region)
pi = 0.8019 Pa (high pressure region)
po = -0.1960 Pa (low pressure region)
N = 128 (fluid cell number on each side)
Mfac = 0.5 (mesh factor)
mu = 0.1 (viscosity)
rho = 1 (density)
Kappa = 1 (spring parameter)
-stokes_ksp_rtol 1.0e-12

## Results
### Vorticity field
Cole
![image](https://github.com/user-attachments/assets/ba4a5fd5-62a6-4de7-a55b-cc22ba223aa1)
![image](https://github.com/user-attachments/assets/f9a42787-3a39-4133-af83-b3552f3e9872)
![image](https://github.com/user-attachments/assets/48d4acbb-f51f-4335-97dc-cd1848559940)
 	 	 
IBAMR	 
![image](https://github.com/user-attachments/assets/87c321c8-b27d-488c-9cc5-b0198bda395d)
![image](https://github.com/user-attachments/assets/3308537b-c9b2-4aa2-814c-094a1701ffc3)
![image](https://github.com/user-attachments/assets/a18f5520-d124-4289-98ad-72937cbbdfca)

Vorticity field at t = 0.05 for the IB method using the IB4, BS4BS3, and BS6BS5 regularized delta functions. Top (Cole’s code), Bottom (IBAMR IB ) 

### Force at T = 0.05  
![image](https://github.com/user-attachments/assets/2624933c-1088-4fa2-b368-995ead3d0938)
![image](https://github.com/user-attachments/assets/219ea7b0-9bc6-4892-adb6-e4b29d4b8157)
![image](https://github.com/user-attachments/assets/3d7fbab9-3e5c-4718-a8ac-7a9a78c0053b)

### Pressure at T = 0.05  
![image](https://github.com/user-attachments/assets/6e7c4d51-6455-4212-921b-5d2b3c1a44a8)
![image](https://github.com/user-attachments/assets/8d7cece9-c58d-4fe4-be70-2428f8d8b660)
![image](https://github.com/user-attachments/assets/b4b92dc8-0836-4149-9dc6-111a060a8000)

### Volume conservation comparison
Cole  
![image](https://github.com/user-attachments/assets/fdaba1f2-dd57-4621-9bfb-34fc37b6bace)

IBAMR  
![image](https://github.com/user-attachments/assets/ca23488d-05ac-4d6b-bf1b-b2134c30630b)




