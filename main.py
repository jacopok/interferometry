#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Mar  5 14:16:33 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm
from os import listdir

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

#par_names = name