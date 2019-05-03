#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  1 21:06:12 2019

@author: jacopo
"""

import initialization as init
import PDF_reader

import matplotlib.pyplot as plt
import numpy as np

"""
To do:
    
    Grafico calibrazione step/angolo (linearità)
    Grafico pdf alpha
    Grafico sinus con minimi segnati, in cui si veda la zona centrale e quella lontana, e i threshold, e parabola di fit
    Grafico di una measure, fondo segnale sottrazione
    Schema errori triangolari -> Qbic
        Dataset con fit sovraimposto (riusciamo a farlo con la ‘nuvola’ di curve di fit, o confidence level region?)
    PDF di γ, dei singoli dataset e della media
    PDF di n_L paraffina, dei singoli dataset e della media
    PDF di n_L glicerina

"""

def step_from_fringes(N, gamma, alpha, index, theta0, N0):
    """
    Inverse function of fringes_difference_medium, still for a generic medium
    """
    Nabs = np.abs(N) - N0
    cos_arg = (index**2 - 1 - (gamma * Nabs + index - 1)**2)/(2*(gamma*Nabs+index-1))
    cos_arg = np.clip(cos_arg, -1, 1)
    theta = (np.sign(N) * np.arccos(cos_arg)) + theta0
    step = theta / alpha
    return(step)
    
def normalize_pdf(pdf):
    #pdf = np.log(1+pdf)
    pdf = pdf / np.max(pdf) /2
    return(pdf)

paraffs = init.quick_measures('p')
steps_bp3p3 = paraffs['bp3-p3'].signal.step_array
fringes_bp3p3 = paraffs['bp3-p3'].signal.fringes_array

params_bp3p3, pdf_bp3p3, names_params_bp3p3 = PDF_reader.reader_pdf('montecarlo/Fit/pa/bp3p3_total_MPDF.txt')
pdf_bp3p3 = normalize_pdf(pdf_bp3p3)

gamma_bp3p3 = 2.68895e-5
alpha_bp3p3 = 51.1928e-6

counter = 0

for par, pdf_par in zip(params_bp3p3, pdf_bp3p3):
    if(pdf_par>1e-9):
        steps = step_from_fringes(fringes_bp3p3, gamma_bp3p3, alpha_bp3p3, *par)
        plt.plot(fringes_bp3p3, steps, alpha = pdf_par)
        counter += 1

plt.scatter(fringes_bp3p3, steps_bp3p3, color = 'red')