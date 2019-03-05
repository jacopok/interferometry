#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm
import itertools

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

name_array_empty_wat = ['fold1551792325',
'fold1551795340',
'fold1551797827']

name_array_wat = ['fold1551800969',
'fold1551802455']

zeros_array_empty_wat = [54, 50, 118]

zeros_array_wat = [59, 76]

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

empty_wat = initialize(name_array_empty_wat, zeros_array_empty_wat)
wat = initialize(name_array_wat, zeros_array_wat)

#try with: par 1, empty_par 0

empty_par[0].output_centered_mc('data/processed/bkg_auto_1.txt')

meas = fdm.measure(empty_par[0], par[1])
meas.subtract_background()
meas.signal.output_centered_mc('data/processed/subtracted_bkg_test.txt')
meas.signal.analyze_fine(55, 6)
meas.signal.output_centered_mc('data/processed/subtracted_bkg_test_centered.txt')

for i, j in itertools.product(range(3), range(2)):    
    wmeas =fdm.measure(empty_wat[i], wat[j])
    wmeas.subtract_background()
    p = wmeas.signal.find_zero_th(55,6)
    g = p[3]
    n = p[4]
    a = (n-1)/(g*n)
    n = 1/(1-a*(2.67e-5))
    x = meas.signal
    s = x.step_array
    fr = x.offset_fringes_th(s, *p)
    plt.plot(s, fr)
    plt.plot(s, x.fringes_array)
    plt.show()
    print(n)