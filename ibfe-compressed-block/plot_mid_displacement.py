import pandas as pd
import matplotlib.pyplot as plt
import os.path
import scienceplots

plt.style.use('science')
MFAC = 0.5
file_path = [
    r'IB4_nu0.4_dY.csv',
    r'IB4_nu-1_dY.csv',
    r'CBS32_nu0.4_dY.csv',
    r'CBS32_nu-1_dY.csv',
    r'CBS43_nu0.4_dY.csv',
    r'CBS43_nu-1_dY.csv'    
]

labels = [
    r"$IB_4 (\nu=0.4)$",        r"$IB_4 (\nu = -1)$", 
    r"$CBS_{32} (\nu = 0.4)$",    r"$CBS_{32} (\nu = -1)$",
    r"$CBS_{43} (\nu = 0.4)$",    r"$CBS_{43} (\nu = -1)$"
]
markers = [ "v",    "p",    
            "v",    "p",
            "v",    "p"]
colors = [(0, 0, 1, 1), (0, 0, 1, 1),  # Different intensities of blue
          (1, 0.6, 0, 1), (1, 0.6, 0, 1),  # Different intensities of orange
          (1, 0, 1, 1), (1, 0, 1, 1.0)]  # Different intensities of magenta

ls = ['-',  '-',  
      '-.', '-.',
      '--', '--']

fig, ax = plt.subplots(figsize=(10, 5))



# Create an inset of the zoomed-in area
axins = ax.inset_axes([0.7, 0.35, 0.3, 0.45])  # [x0, y0, width, height]
axins1 = ax.inset_axes([0.2, 0.35, 0.3, 0.45])  # [x0, y0, width, height]
for i, file in enumerate(file_path):
    if os.path.exists(file):
        print(i, file)
        data = pd.read_csv(file, sep=",")  # Use whitespace as delimiter
        y = data['avg(dX_1)']
        x = data['Time']
        # Sample every 10th point
        x = x[::5]
        y = y[::5]        
        ax.plot(x, y, linestyle = ls[i], marker=markers[i], label = labels[i], color=colors[i])
        axins.plot(x, y, linestyle = ls[i], marker=markers[i], label = labels[i], color=colors[i])
        axins1.plot(x, y, linestyle = ls[i], marker=markers[i], label = labels[i], color=colors[i])

    else:
        ax.plot([],[], color ="white", label = " ")



# Define the zoomed-in area
x1, x2, y1, y2 = 495, 500, -4.25, -4.05
axins.set_xlim(x1, x2); axins.set_ylim(y1, y2)

x1, x2, y1, y2 = 100, 120, -4.25, -4.05
axins1.set_xlim(x1, x2); axins1.set_ylim(y1, y2)

# Set the color of the axis labels
axins.xaxis.label.set_color('grey'); axins.yaxis.label.set_color('grey')
axins1.xaxis.label.set_color('grey'); axins.yaxis.label.set_color('grey')

# Set the color of the tick marks
axins.tick_params(axis='x', colors='grey'); axins.tick_params(axis='y', colors='grey')
axins1.tick_params(axis='x', colors='grey'); axins1.tick_params(axis='y', colors='grey')

# Set the color of the axis spines
axins.spines['left'].set_color('grey'); axins.spines['bottom'].set_color('grey')
axins.spines['top'].set_color('grey') ;axins.spines['right'].set_color('grey')
axins1.spines['left'].set_color('grey'); axins1.spines['bottom'].set_color('grey')
axins1.spines['top'].set_color('grey') ; axins1.spines['right'].set_color('grey')
# Add a rectangle to indicate the zoomed-in area on the main plot
ax.indicate_inset_zoom(axins, edgecolor="grey"); 
ax.indicate_inset_zoom(axins1, edgecolor="grey")


plt.xlabel('t')
plt.ylabel(r'$dY$')

plt.title('Displacement comparison of Compressed Block \n (Nodal Coupling IFED)')
plt.xlim([0, 500])  
plt.tight_layout()
plt.legend(ncol = 3,  loc='best',)

# Show the plot
plt.show()
