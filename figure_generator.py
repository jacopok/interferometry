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

import warnings
warnings.filterwarnings("ignore")

from matplotlib import rc
rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)
rc('text.latex', preamble=r'''\usepackage{amsmath}
          \usepackage{physics}
          ''')

"""
To do:

    Grafico calibrazione step/angolo (linearità) (Leonardo)
    Grafico pdf alpha (Leonardo)
    Grafico sinus con minimi segnati, in cui si veda la zona centrale e quella lontana,
        e i threshold, e parabola di fit (Leonardo (no parabola?))

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
    pdf = pdf / np.max(pdf) /5
    return(pdf)

paraffs = init.quick_measures('p')
steps_bp3p3 = paraffs['bp3-p3'].signal.step_array
fringes_bp3p3 = paraffs['bp3-p3'].signal.fringes_array

paraffs['bp3-p3'].calculate_hw()
errors_bp3p3 = paraffs['bp3-p3'].signal.hw_array / np.sqrt(6)

params_bp3p3, pdf_bp3p3, names_params_bp3p3 = PDF_reader.reader_mpdf('montecarlo/Fit/pa/bp3p3_total_MPDF.txt')

#NB: the first to vary should be n_l, then theta_0, then N_0

pdf_bp3p3 = normalize_pdf(pdf_bp3p3)

#gamma_bp3p3 = 2.68895e-5
#alpha_bp3p3 = 51.1928e-6

@timeit
def plot_cloud(fringes, steps, params, pdf, errors=None, radius=None, xlims=None,ylims=None,
    figname=None, gamma=2.68895e-5, alpha=51.1928e-6, show=True, **kwargs):

    fig = plt.figure(1)

    if(radius):
        mask=np.abs(fringes)<radius
    else:
        mask1 = fringes>xlims[0]
        mask2 = fringes<xlims[1]
        mask = mask1*mask2

    plt.ylabel('Angle [rad]')
    plt.xlabel('Fringe number [1]')

    if(xlims):
        test_fringes = np.linspace(xlims[0], xlims[1], num=200)
    else:
        test_fringes = fringes[mask]

    counter = 0
    for par, pdf_par in zip(params, pdf):
        if(pdf_par>1e-5):
            steps_fit = step_from_fringes(test_fringes, gamma, alpha, *par)
            plt.plot(test_fringes, steps_fit, alpha=pdf_par, c='b')
            counter += 1

    if(errors is not None):
        plt.errorbar(fringes[mask], steps[mask], yerr=errors[mask],**kwargs, label='Data points')
    else:
        plt.plot(fringes[mask], steps[mask], 'ro', label = 'Data points')

    if(kwargs['ms'] is not None):
        plt.legend(markerscale= 5 / kwargs['ms'])
    else:
        plt.legend()
    if(show==True):
        plt.show()
    else:
        if(not figname):
            raise ValueError('Must provide name for figure!')
        if(exists(figname)):
            remove(figname)
        fig.savefig(figname, format='pdf')
        plt.close('all')
    return counter

if __name__ == '__main__':

    plot_cloud(fringes_bp3p3, steps_bp3p3, params_bp3p3, pdf_bp3p3,errors = errors_bp3p3,
        radius = 20,  figname =  'figs/fit_cloud_bp3p3.pdf', show=False,
        fmt='ro', ms=2, capsize=1.5, elinewidth=1, markeredgewidth=0.5)

    xlims = [-10.04, -9.96]
    ylims = [-800, -750]


    plot_cloud(fringes_bp3p3, steps_bp3p3, params_bp3p3, pdf_bp3p3, errors = errors_bp3p3,
        xlims=xlims, ylims=ylims, figname = 'figs/fit_zoom_bp3p3.pdf', show=False,
        fmt='ro', ms=10, capsize=5, elinewidth=2, markeredgewidth=2)

    fig = plt.figure(2)
    paraffs['bp3-p3'].plot(radius=0.09)
    fig.savefig('figs/bp3p3_measure.pdf', format = 'pdf')
    plt.close('')
