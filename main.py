#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm
from os import listdir
from itertools import product

data_dir = './data/belly/'
proc_dir = './data/processed/'

names = listdir(data_dir)

names = [name for name in names if 'sinus' not in name]

def initialize(name_array):
    dataset_array = []
    for name in name_array:
        data = fdm.dataset(data_dir + name)
        data.set_zero(0)
        data.analyze_fine(ignore_radius=0, fringes_radius=1000)
        dataset_array.append(data)
    return(dataset_array)

bkg_names = [n for n in names if 'b' in n]
sig_names = [n for n in names if n not in bkg_names]
a_names = [n for n in sig_names if 'a' in n]
p_names = [n for n in sig_names if 'p' in n]

bkg_datasets = initialize(bkg_names)
sig_datasets = initialize(sig_names)
a_datasets = initialize(a_names)
p_datasets = initialize(p_names)

n_array = []
for bkg, sig in product(bkg_datasets, sig_datasets):
    mea = fdm.measure(bkg, sig)
    mea.subtract_background(use_data=True)
    signal = mea.signal
    bkg_name = bkg.filename.split('.')[-2].split('/')[-1]
    sig_name = sig.filename.split('.')[-2].split('/')[-1]
    signal.output_centered_mc(proc_dir + bkg_name + sig_name + '.txt')
    """
    print(bkg.filename,  sig.filename)
    p = signal.find_zero_th()
    a = (p[3]-1) / (p[2] * p[3])
    print(1/(1-a * 2.67e-5))
    n_array.append(1/(1-a * 2.67e-5))
    """