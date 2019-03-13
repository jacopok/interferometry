#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Mar 13 10:34:36 2019

@author: jacopo
"""

import Fringes_dataset_manager as fdm
import initialization as init
from numpy.random import uniform
from numpy.random import normal

def theta_from_fringes_medium(N, index, gamma):
    """
    Inverse function of fringes_difference_medium, still for a generic medium
    """    
    Nabs = np.abs(N)
    theta = (np.sign(N) * np.arccos((index**2 - 1 - (gamma * Nabs + index - 1)**2)
        /(2*(gamma*Nabs+index-1))))
    return(theta)
    
thickness = 1e-2 #m
wavelength = 532e-9 #m
index = 1.33

gamma = wavelength /2 /thickness

cfac = 42.6e-6

Nsims = 50
maxfringe = 80
Ndata = 2*maxfringe

np.random.seed(10414)

fringes = np.arange(-80, 80)
angles = theta_from_fringes_medium(fringes, index, gamma)

def dirty_angles(angles, step_noise=cfac, gain_noise=10/6151):
    noise1 = uniform(-step_noise, step_noise, Ndata)
    noise2 = normal(loc=1,scale=gain_noise, size=1)
    angles += noise1
    angles *= noise2
    return(angles)
    
def dataset_sim(fringes, index, gamma, cfac=cfac):
    angles = theta_from_fringes_medium(fringes, index, gamma)
    angles = dirty_angles(angles)
    steps = angles / cfac
    data = fdm.dataset()
    data.step_array = steps
    data.fringes_array = fringes
    data.analyze_fine()
    return(data)

def measure_sim(fringes, index_b, gamma_b, index_l, gamma_l, cfac=cfac):
    bkg = dataset_sim(fringes, index_b, gamma_b, cfac)
    sig = dataset_sim(fringes, index_l, gamma_l, cfac)
    mea = fdm.measure(bkg, sig)
    mea.subtract_background(use_data=True)
    return(mea)

index_array = []

for s in seed_array:
    data = dataset_sim(fringes, index, gamma)
    p = data.fit()
    a = (p[3]-1) / (p[2] * p[3])
    index_array.append(1/(1-a * gamma))
    #d.plot(residuals=True, label = str(n))
#plt.legend()