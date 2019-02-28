import Fringes_dataset_manager as fdm
import sys
import os.path

"""
This script takes in input the name of the file to analyze, and
the number of the zero fringe.
It then creates two files with the positive and negative fringe orders
separated and normalized.
"""

filename = sys.argv[1]
zero_fringe = sys.argv[2]

data = fdm.dataset(filename, flipped=True)
data.set_zero(zero_fringe)

data.analyze_fine(fringes_radius=20)

name = os.path.splitext(filename)[0]
out_name = name + "_centered.txt"

data.output_centered(out_name)

out_name_mc = name + "_centered_mc.txt"

data.output_centered_mc(out_name_mc)