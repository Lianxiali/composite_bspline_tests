# Note
1. run matlab code (provided by Cole Gruninger) to generate vertex file
2. run the script _run.sh_ for a batch run.

# [1] Steady, 2D Poiseuille flow
Plane Poiseuille flow is defined as a steady, laminar flow of a viscous fluid between two horizontal parallel plates separated by a distance, H. 
Flow is induced by a pressure gradient across the length of the plates, LL, and is characterized by a 2D parabolic velocity profile symmetric about 
the horizontal mid-plane as illustrated in Fig 2.1.

![image](https://github.com/user-attachments/assets/3bcd9de4-f24e-4893-99f5-081005624c57)
[Fig 2.1 Plane Poiseuille flow between two flat plates of length L, separated by a distance H](https://mfix.netl.doe.gov/doc/vvuq-manual/main/html/fluid/fld-01.html)


![image](https://github.com/user-attachments/assets/78c19776-a927-4bc3-8e97-4fcfe3642985)
Computational domain, pressure boundary conditions, and mesh

![image](https://github.com/user-attachments/assets/3eb3549f-b718-4c2f-b9b8-71400068824b)
Force distribution (IB3)  

![image](https://github.com/user-attachments/assets/8ecd4b44-bbe4-4e86-93a4-4a8e3ba52bb4)
Velocity distribution and vector (IB3)

![image](https://github.com/user-attachments/assets/10aab349-e384-41ce-8cb3-4fd8b471b2a7)

**Boundary Layer thickness in ascending order**  
Analytical = 0 < CBS21 < CBS32 < CBS43 < IB3<CBS54 < CBS65 < IB4

**Max velocity in descending order**  
Analytical =1 > CBS21 > IB4 > CBS54 > CBS65 > IB3 > CBS43 > CBS32

![image](https://github.com/user-attachments/assets/7d8ef24b-7767-454d-b3da-79d2321d9748)
![image](https://github.com/user-attachments/assets/d22e374d-1628-436d-b05b-37d662736eaa)


# [2] Steady, Slanted 2D Poiseuille flow

![image (12)](https://github.com/user-attachments/assets/cb6404fd-3731-4478-aebd-45a5120710b6)
![image (13)](https://github.com/user-attachments/assets/26cf1a79-e5e6-4dec-93f0-b8bc48270cf4)
![image (14)](https://github.com/user-attachments/assets/22fa7573-721b-49c6-add6-46421d6e1894)
![image (15)](https://github.com/user-attachments/assets/fd867632-5462-43ae-9f53-c5e2d4ba0010)
![image (16)](https://github.com/user-attachments/assets/d5647d13-5768-4699-a084-c8264e88f9aa)





