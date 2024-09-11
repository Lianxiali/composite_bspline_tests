import os
import glob
import re

# List of cases
cases = ["viz_IB_3", "viz_IB_4", 
"viz_PIECEWISE_LINEAR", "viz_BSPLINE_3", "viz_BSPLINE_4",  "viz_BSPLINE_5", "viz_BSPLINE_6",
"viz_DISCONTINUOUS_LINEAR", "viz_COMPOSITE_BSPLINE_32", "viz_COMPOSITE_BSPLINE_43", "viz_COMPOSITE_BSPLINE_54", "viz_COMPOSITE_BSPLINE_65"]

for case in cases:
    # Define paths for each case
    viz_path = f"./{case}/visit*/"  # Data folder path using the current case
    out_file_name = f"./{case}paraview.samrai.series"  # Output file name for the current case

    # Initialize data arrays
    arrdata = [0]  # Array for time steps (probe data), starts with 0
    arrfolder = []  # Array to store folder names

    # Get folder names matching the VizPath pattern
    arrfolder = glob.glob(viz_path)

    # Check if the output file already exists and delete it
    if os.path.isfile(out_file_name):
        os.remove(out_file_name)

    # Sort the folders in natural order
    arrfolder.sort(key=lambda x: [int(text) if text.isdigit() else text.lower() for text in re.split('([0-9]+)', x)])

    # Write the JSON content to the output file
    with open(out_file_name, 'w') as outfile:
        outfile.write("{\n")
        outfile.write('  "file-series-version" : "1.0",\n')
        outfile.write('  "files" : [\n')

        count = 0  # Counter for time steps
        for i in arrfolder:
            # Reformat the folder name and replace '/' with '\\'
            var = '\\\\'.join(re.split(r'/', os.path.normpath(i))[-2:])
            print(var)  # Debug print to check the reformatted path
            
            # Create the file path string
            var2 = f"{var}\\\\summary.samrai"
            # Write the entry to the JSON series file
            outfile.write(f'    {{ "name" : "{var2}", "time" : {count} }},\n')
            count += 1

        # Close the JSON structure
        outfile.write("  ]\n")
        outfile.write("}\n")
