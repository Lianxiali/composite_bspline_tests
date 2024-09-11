import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

# Define the color cycle
plt.rcParams['axes.prop_cycle'] = plt.cycler(color=plt.cm.tab10.colors)

file_path = [
    'IB_3.U0000.curve',
    'IB_4.U0000.curve',
    'COMPOSITE_BSPLINE_21.U0000.curve',
    'COMPOSITE_BSPLINE_32.U0000.curve',
    'COMPOSITE_BSPLINE_43.U0000.curve',
    'COMPOSITE_BSPLINE_54.U0000.curve',
    'COMPOSITE_BSPLINE_65.U0000.curve',
    'BSPLINE_2.U0000.curve',
    'BSPLINE_3.U0000.curve',
    'BSPLINE_4.U0000.curve',
    'BSPLINE_5.U0000.curve',
    'BSPLINE_6.U0000.curve'    
]
labels = [    
    r"$IB_3$",    
    r"$IB_4$",    
    r"$CBS_{21}$",    
    r"$CBS_{32}$",
    r"$CBS_{43}$",    
    r"$CBS_{54}$",    
    r"$CBS_{65}$"
    r"$BS_{2}$",    
    r"$BS_{3}$",
    r"$BS_{4}$",    
    r"$BS_{5}$",    
    r"$BS_{6}$"    
    ]
markers = [    "o",    "s",    "^",    "v",    "p",    "*",    "+"]
colors = ['blue', 'green', 'red', 'cyan', 'magenta', 'purple', 'orange']
D = 1
Umax = 1
# Read the data from the text file, skipping the comment line
for i, file in enumerate(file_path):
    data = pd.read_csv(file, header=None, names=['y', 'U'], delim_whitespace=True, comment='#')
    if (i>6):
        ls = "--"
    else:
        ls = "-"
    
    plt.plot(data['y'], data['U']/Umax, linestyle = ls, marker=markers[i], label = labels[i], color=colors[i])

# Define the parameters
y = np.linspace(0, 5, 500)  # 100 points between 0 and 5
U_MAX = 1
H = 5
D = 1
mu = 0.01
A = (H + D) / 2
B = (H - D) / 2
C = -4 * U_MAX / (D * D)

# Initialize U as an array of zeros with the same shape as y
U = np.zeros_like(y)

# Compute the velocity profile
for i in range(len(y)):
    if abs(y[i] - 0.5 * H) <= D / 2:
        U[i] = C * (y[i] - A) * (y[i] - B)  # Update U at index i
    else:
        U[i] = 0
# plt.plot(y, U, color='black', marker='', label="Analytical")
# Labeling the axes
plt.xlabel(r'$y/D$')
plt.ylabel(r'$u/U_{max}$')
plt.title(r'Velocity profile comparison of slanted ($\pi/12$) Hagen–Poiseuille flow'+' \n with different IB kernals')

# Show the plot
plt.legend(ncol=2)
plt.tight_layout()
plt.show()
