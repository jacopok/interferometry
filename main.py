#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm
from os import listdir
from itertools import product

names = listdir('./data/belly')

names = [name for name in names if 'sinus' not in name]

def initialize(name_array):
    dataset_array = []
    for name in name_array:
        data = fdm.dataset('data/belly/' + name)
        data.set_zero(0)
        data.analyze_fine(ignore_radius=0, fringes_radius=1000)
        dataset_array.append(data)
    return(dataset_array)

bkg_names = [n for n in names if 'b' in n]
sig_names = [n for n in names if n not in bkg_names]

bkg_datasets = initialize(bkg_names)
sig_datasets = initialize(sig_names)

for bkg, sig in product(bkg_datasets, sig_datasets):
    mea = fdm.measure(bkg, sig)
    mea.subtract_background(use_data=False)
    signal = mea.signal
    signal.plot_center(2000)
    plt.show()
    print(bkg.filename,  sig.filename)