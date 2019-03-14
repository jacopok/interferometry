#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Mar 14 14:09:11 2019

@author: jacopo
"""

thickness = 1e-2 #m
wavelength = 532e-9 #m
index = 1.33

gamma = wavelength /2 /thickness

cfac = 42.6e-6

Nsims = 50
maxfringe = 80
Ndata = 2*maxfringe

np.random.seed(10414)

index_array = []

for s in range(Nsims):
    data = measure_sim(fringes, 1.5, 3e-5, 1.33, gamma)
    p = data.signal.fit()
    a = (p[3]-1) / (p[2] * p[3])
    index_array.append(1/(1-a * gamma))
    print('this')
