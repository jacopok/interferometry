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
proc_dir = './data/processed_qbic/'

names = listdir(data_dir)
names = [name for name in names if 'sinus' not in name]
#keeps only the names which do not contain 'sinus'

def initialize(name, data_dir=data_dir):
    """
    Creates a dataset from the data in the file "name" in the folder "data_dir"
    """
    data = fdm.dataset(filename=data_dir + name)
    data.set_zero(0)
    data.analyze_fine()
    return(data)
    
def create_measures(sig_name_array, bkg_name_array, use_data=True):
    """
    Takes in input two arrays of strings, containing the names of the
    files from which to read the signal and background respectively.
    
    Creates a dictionary of measures, one for each combination
    of signal and background, labelled by
    'bkg_name-sig_name'
    """
    measure_dict = {}
    for sig, bkg in product(sig_name_array, bkg_name_array):
        data_sig = initialize(sig)
        data_bkg = initialize(bkg)
        mea = fdm.measure(data_bkg, data_sig)
        mea.subtract_background(use_data)
        mea.name = data_bkg.name + '-' + data_sig.name
        measure_dict[mea.name] = mea
    return(measure_dict)

def get_names(liquid, names, bkg='b',):
    """
    Assuming the names in "names" contain the string bkg
    only if they are background, and the string liquid only if
    they correspond to that liquid,
    returns arrays of signal names and background names.
    """

    liquid_names = [n for n in names if liquid in n]
    sig_names = [n for n in liquid_names if bkg not in n]
    bkg_names = [n for n in liquid_names if bkg in n]
    return(sig_names, bkg_names)

def output_mc(measure_dict):
    """
    Outputs to proc_dir the signal (gross signal-background)
    in montecarlo-ready format
    """
    for key, measure in measure_dict.items():
        measure.calculate_hw()
        measure.output_mc_triang(proc_dir + key + '.txt')

def quick_measures(l, use_data=True):
    return(create_measures(*get_names(l, names), use_data))


def get_n(dataset, gamma = 2.66e-5, **kwargs):
    p, perr = dataset.fit(p0=(0,0,gamma,1.3), bounds=([-4000, 0, gamma-1e-10, 1],
                              [4000, 1, gamma+1e-10, 3]) ,**kwargs)
    return(p, perr)

def get_gamma(dataset, n, gamma=2.66e-5, **kwargs):
    
    p, perr = dataset.fit(p0=(0,0,gamma, n), bounds=([-4000, 0, gamma/5, n-1e-10],
                              [4000, 1, 2*gamma, n+1e-10]) ,**kwargs)
    return(p, perr)

    