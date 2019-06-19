#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  1 21:06:12 2019

@author: jacopo, alessandro
"""

import initialization as init
import PDF_reader

import matplotlib.pyplot as plt
import numpy as np
from os import remove
from os.path import exists

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

def timeit(method):
    def timed(*args, **kw):
        from time import time
        ts = time()
        result = method(*args, **kw)
        te = time()
        print('%r  %2.2f s' % \
                  (method.__name__, (te - ts)))
        return result
    return timed

def step_from_fringes(N, gamma, alpha, N0, theta0, index):
    """
    Inverse function of fringes_difference_medium, still for a generic medium
    """
    Nabs = np.abs(N) - N0
    cos_arg = (index**2 - 1 - (gamma * Nabs + index - 1)**2)/(2*(gamma*Nabs+index-1))
    cos_arg = np.clip(cos_arg, -1, 1)
    theta = (np.sign(N) * np.arccos(cos_arg)) + theta0
    step = theta / alpha
    return step
    
def normalize_pdf(pdf):
    #pdf = np.log(1+pdf)
    pdf = pdf / np.max(pdf) /2
    return(pdf)

paraffs = init.quick_measures('p')
steps_bp3p3 = paraffs['bp3-p3'].signal.step_array
fringes_bp3p3 = paraffs['bp3-p3'].signal.fringes_array

params_bp3p3, pdf_bp3p3, names_params_bp3p3 = PDF_reader.reader_mpdf('montecarlo/Fit/pa/bp3p3_total_MPDF.txt')

#NB: the first to vary should be n_l, then theta_0, then N_0

pdf_bp3p3 = normalize_pdf(pdf_bp3p3)

#gamma_bp3p3 = 2.68895e-5
#alpha_bp3p3 = 51.1928e-6

@timeit
def plot_cloud(fringes, steps, params, pdf, figname=None, gamma=2.68895e-5, alpha=51.1928e-6, show=True):
    counter = 0
    for par, pdf_par in zip(params, pdf):
        if(pdf_par>1e-5):
            steps_fit = step_from_fringes(fringes, gamma, alpha, *par)
            plt.plot(fringes, steps_fit, alpha=pdf_par, c='b')
            counter += 1

    plt.scatter(fringes, steps, color = 'red')

    if(show==True):
        plt.show()
    else:
        if(not figname):
            raise ValueError('Must provide name for figure!')
        if(exists(figname)):
            remove(figname)
        plt.savefig(figname, dpi=1000)
    return counter

#plot_cloud(fringes_bp3p3, steps_bp3p3, params_bp3p3, pdf_bp3p3, figname =  'figs/fit_cloud_bp3p3.png')

