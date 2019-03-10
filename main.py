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

def initialize(name):
    data = fdm.dataset(data_dir + name)
    data.set_zero(0)
    data.analyze_fine(ignore_radius=0, fringes_radius=1000)
    return(data)
    
def create_measures(sig_name_array, bkg_name_array):
    measure_dict = {}
    for sig, bkg in product(sig_name_array, bkg_name_array):
        data_sig = initialize(sig)
        data_bkg = initialize(bkg)
        mea = fdm.measure(data_bkg, data_sig)
        mea.subtract_background()
        mea_name = (int(data_bkg.name[-1]), int(data_sig.name[-1]))
        measure_dict[mea_name[0]] = measure_dict.get(mea_name[0], {})
        measure_dict[mea_name[0]][mea_name[1]] = mea
    return(measure_dict)

def get_names(liquid, names):
    bkg = 'b'
    liquid_names = [n for n in names if liquid in n]
    sig_names = [n for n in liquid_names if bkg not in n]
    bkg_names = [n for n in liquid_names if bkg in n]
    return(sig_names, bkg_names)

def output_mc(measure_dict):
    for key, measure in measure_dict.items():
        sig = measure.signal
        bkg_name, sig_name = key
        sig.output_mc(proc_dir + bkg_name + sig_name + '.txt')

def quick_measures(l):
    return(create_measures(*get_names(l, names)))

#for l in ['p', 'a']:
#    output_mc(create_measures(*get_names(l, names)))

def get_n(meas, gamma = 2.67e-5):
    p = meas.signal.fit()
    a = (p[3]-1) / (p[2] * p[3])
    return(1/(1-a * gamma))