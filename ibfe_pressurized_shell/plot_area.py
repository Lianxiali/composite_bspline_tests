import pandas as pd
import matplotlib.pyplot as plt

file_path = [
    'IB_3_volume.curve',
    'IB_4_volume.curve',
    'IB_5_volume.curve',
    'IB_6_volume.curve',
    'PIECEWISE_LINEAR_volume.curve',
    'BSPLINE_3_volume.curve',
    'BSPLINE_4_volume.curve',
    'BSPLINE_5_volume.curve',
    'BSPLINE_6_volume.curve',
    'DISCONTINUOUS_LINEAR_volume.curve',
    'COMPOSITE_BSPLINE_32_volume.curve',
    'COMPOSITE_BSPLINE_43_volume.curve',
    'COMPOSITE_BSPLINE_54_volume.curve',
    'COMPOSITE_BSPLINE_65_volume.curve'
]
labels = [
    r"$IB_3$",        r"$IB_4$",        r"$IB_5$",        r"$IB_6$",
    r"$BS_2$",        r"$BS_3$",        r"$BS_4$",        r"$BS_5$",        r"$BS_6$",
    r"$CBS_{21}$",    r"$CBS_{32}$",    r"$CBS_{43}$",    r"$CBS_{54}$",    r"$CBS_{65}$"
]
markers = [ "v",    "p",    "*",    "+",    
            "^",    "v",    "p",    "*",    "+",
            "^",    "v",    "p",    "*",    "+"]
colors = [(0, 0, 1, 1), (0, 0, 1, 1), (0, 0, 1, 0.8), (0, 0, 1, 1.0),  # Different intensities of blue
          (1, 0, 1, 1), (1, 0, 1, 1), (1, 0, 1, 1), (1, 0, 1, 0.8), (1, 0, 1, 1.0),  # Different intensities of magenta
          (1, 0.6, 0, 1), (1, 0.6, 0, 1), (1, 0.6, 0, 1), (1, 0.6, 0, 0.8), (1, 0.6, 0, 1.0)]  # Different intensities of orange
ls = ['-',  '-',  '-',  '-',
      '--', '--', '--', '--', '--',
      '-.', '-.', '-.', '-.', '-.']
plt.figure(figsize=(10, 6))
plt.semilogy([],[], color ="white", label = " ")
for i, file in enumerate(file_path):
    data = pd.read_csv(file, delim_whitespace=True)  # Use whitespace as delimiter
    area = data.iloc[:, 1]
    area = (area - area.iloc[0]) / area.iloc[0]

    t = data.iloc[:, 0]
    area = abs(area)
    # Sample every 10th point
    x = t[::20]
    y = area[::20]
    plt.semilogy(x, y, linestyle = ls[i], marker=markers[i], label = labels[i], color=colors[i])

plt.xlabel('t')
plt.ylabel(r'$\Delta V(t, X)=\frac{|V-V_0|}{|V_0|}$')

plt.title('Volume conservation comparison of pressurized elastic shell \n (Nodal IBFE, MFAC$=1)$ ')
# plt.ylim([1e-9, 1e-0])  
plt.xlim([0, 1])  

plt.legend(ncol = 3,  bbox_to_anchor=(0.3, -0.2, 0.5, 0.1), loc='upper left',)

# Show the plot
plt.tight_layout()

plt.show()
