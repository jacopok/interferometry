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
#keeps only the names which do not contain 'sinus'

def initialize(name):
    """
    Creates a dataset from the data in the file "name" in the folder "data_dir"
    """
    data = fdm.dataset(data_dir + name)
    data.set_zero(0)
    data.analyze_fine()
    return(data)
    
def create_measures(sig_name_array, bkg_name_array):
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
        mea.subtract_background(use_data=True)
        mea_name = data_bkg.name + '-' + data_sig.name
        measure_dict[mea_name] = mea
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
        sig = measure.signal
        sig.output_mc(proc_dir + key + '.txt')

def quick_measures(l):
    return(create_measures(*get_names(l, names)))


def get_n(meas, gamma = 2.67e-5):
    p = meas.signal.fit()
    a = (p[3]-1) / (p[2] * p[3])
    return(1/(1-a * gamma))