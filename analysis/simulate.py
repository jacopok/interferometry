#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 14 14:09:11 2019

@author: jacopo
"""

from simulation_functions import *
import Fringes_dataset_manager as fdm
import initialization as init
import numpy as np

thickness_interior = 1e-2 #m
thickness_glass = 6e-3 #m
wavelength = 532e-9 #m
#index = 1.33

index_b = 1.5
gamma_b = wavelength /2 /thickness_glass
index_l = 1.426
gamma_l = wavelength /2 /thickness_interior

cfac = 5.11928e-5

params = (index_b, gamma_b, index_l, gamma_l, cfac)

Nsims = 5
maxfringe = 80
Ndata = 2*maxfringe

np.random.seed(10414)

measure_dict = {}

for s in range(Nsims):
    index_b, gamma_b, index_l, gamma_l, cfac = params
    data = measure_sim(maxfringe, index_b, gamma_b, index_l, gamma_l, cfac)
    name = 'simulated_' + str(s)
    measure_dict[name] = data
    
init.output_mc(measure_dict)