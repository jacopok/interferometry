#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm

name_array = ['fold1551439624',
'fold1551441226',
'f20190304va',
'fold1551694989',
'fold1551697620',
'fold1551779770',
'fold1551781255',
'fold1551783240']

contents_array = ['Vuota1f',
'Vuota1b',
'Vuota2f',
'Vuota2b',
'Paraffina1f',
'Paraffina1b',
'Paraffina2f',
'Paraffina2b']

zeros_array = [1,1,1,1,1,1,1,1]

for name, content, zero in zip(name_array, contents_array, zeros_array):
    data = fdm.dataset('data/MinsAndSinus/' + name + '_mins.gimli', flipped = True)
    data.set_zero(zero)
    data.analyze_fine()
    data.output_centered_mc('data/processed/' + content + '.txt')