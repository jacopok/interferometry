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

thickness = 1e-2 #m
wavelength = 532e-9 #m
index = 1.33

gamma = wavelength /2 /thickness

index_b = 1.5
gamma_b = gamma # to fix!!
index_l = 1.426
gamma_l = gamma

#cfac = 42.6e-6
cfac = 5.11928e-5

Nsims = 50
maxfringe = 80
Ndata = 2*maxfringe

np.random.seed(10414)

for s in range(Nsims):
    data = measure_sim(maxfringe, index_b, gamma_b, index_l, gamma_l, cfac)
    