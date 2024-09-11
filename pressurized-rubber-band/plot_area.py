import pandas as pd
import matplotlib.pyplot as plt


file_path = [
    'IB_4tracer.area',
    'DISCONTINUOUS_LINEARtracer.area',
    'COMPOSITE_BSPLINE_32tracer.area',
    'COMPOSITE_BSPLINE_43tracer.area',
    'COMPOSITE_BSPLINE_54tracer.area',
    'COMPOSITE_BSPLINE_65tracer.area'
]
labels = [
    r"$IB_4$",
    r"$CBS_{21}$",
    r"$CBS_{32}$",
    r"$CBS_{43}$",
    r"$CBS_{54}$",
    r"$CBS_{65}$"
]
for i, file in enumerate(file_path):
    data = pd.read_csv(file, delim_whitespace=True)  # Use whitespace as delimiter
    area = data['area']
    area = (area -area[0])/area[0]
    plt.semilogy(data['CurrentTime'], abs(area), marker='', label = labels[i])

plt.xlabel('t')
plt.ylabel(r'$\Delta A(t, X_{tracer})=\frac{|A-A_0|}{|A_0|}$')

plt.title('Volume conservation comparison \n of pressurized elastic band \n (MFAC$_{IB}=0.5$ MFAC$_{tracer}$=0.025)')
plt.ylim([1e-18, 1e-4])  
plt.tight_layout()
plt.legend()

# Show the plot
plt.show()
