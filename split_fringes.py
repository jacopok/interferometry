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
data.split()

name = os.path.splitext(filename)[0]
out_names = [name + "_positive.txt", name + "_negative.txt"]

data.output_split(out_names)