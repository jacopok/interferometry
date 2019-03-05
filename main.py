#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm

name_array_empty_par = ['f20190304va',
'fold1551694989']

label_array_empty_par = ['Vuota1_paraffina', 'Vuota2_paraffina', 'Vuota3_paraffina']

name_array_par = ['fold1551697620',
'fold1551779770',
'fold1551781255',
'fold1551783240']

label_array_par = ['Par1', 'Par2', 'Par3', 'Par4']

zeros_array_empty_par = [33, 77]
zeros_array_par = [125, 67, 82, 140]

def initialize(name_array, zeros_array):
    dataset_array = []
    for name, zero in zip(name_array, zeros_array):
        data = fdm.dataset('data/MinsAndSinus/' + name + '_mins.gimli')
        data.set_zero(zero)
        data.analyze_fine(ignore_radius=6, fringes_radius=55)
        dataset_array.append(data)
        #data.output_centered_mc('data/processed/' + content + '.txt')
    return(dataset_array)

names_merge_1 = ['fold1551439624', 'fold1551441226']

def initialize_merge(names, zero):
    tomerge = fdm.dataset('data/MinsAndSinus/' + names[0] + '_mins.gimli')
    tomerge_1 = fdm.dataset('data/MinsAndSinus/' + names[1] + '_mins.gimli')
    tomerge.join(tomerge_1)
    tomerge.set_zero(zero)
    tomerge.analyze_fine(55, 6)
    return(tomerge)

empty_par_0 = [initialize_merge(names_merge_1, zero=93)]
empty_par_1 = initialize(name_array_empty_par, zeros_array_empty_par)
empty_par = empty_par_0 + empty_par_1
par = initialize(name_array_par, zeros_array_par)

#try with: par 1, empty_par 0

empty_par[0].output_centered_mc('data/processed/bkg_auto_1.txt')

meas = fdm.measure(empty_par[0], par[1])
meas.subtract_background()
meas.signal.output_centered_mc('data/processed/subtracted_bkg_test.txt')
meas.signal.analyze_fine(55, 6)
meas.signal.output_centered_mc('data/processed/subtracted_bkg_test_centered.txt')