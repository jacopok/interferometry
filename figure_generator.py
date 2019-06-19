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

# import matplotlib
# matplotlib.use("TKAGG")

#
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import rc
rc('font',**{'family':'serif','serif':['Palatino']})
rc('text', usetex=True)
rc('text.latex', preamble=r'''\usepackage{amsmath}
          \usepackage{physics}
          ''')
from matplotlib import rcParams
rcParams['axes.labelpad'] = 8


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

    fig = plt.figure()

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

#@timeit
def find_point_plot(xlims, pdf_location, filename, **kwargs):
    mask1 = fringes_bp3p3 > xlims[0]
    mask2 = fringes_bp3p3 < xlims[1]
    mask = mask1 * mask2
    fringe = fringes_bp3p3[mask]
    step = steps_bp3p3[mask]
    pdf_steps, pdf_values = PDF_reader.reader_pdf(pdf_location)

    plot3d(fringe, step, params_bp3p3, pdf_bp3p3, pdf_steps, pdf_values,
        figname=filename, xlims=xlims, **kwargs)


def plot3d(fringe, step, params, pdf, pdf_steps, pdf_values,
           figname=None, gamma=2.68895e-5, alpha=51.1928e-6, show=True, **kwargs):

    fig = plt.figure()
    ax = Axes3D(fig)

    ax.set_xlabel('Fringes [1]', linespacing = 3)
    ax.set_ylabel('Steps [1]', linespacing = 3)
    ax.set_zlabel('Data point PDF [1/step]', linespacing = 3)

    ax.plot(fringe, step, 0, 'ro', label='Data points')

    xlims = kwargs['xlims']
    # to generalize
    test_fringes = np.linspace(*xlims)

    for par, pdf_par in zip(params, pdf):
        if(pdf_par>1e-5):
            steps_fit = step_from_fringes(test_fringes, gamma, alpha, *par)
            plt.plot(test_fringes, steps_fit, pdf_par/20, alpha=2*pdf_par, c='b')

    for s, p in zip(pdf_steps, pdf_values):
       ax.plot([fringe, fringe], [s, s], [0, p], color='green')

    # plt.tight_layout()
    if(figname):
        fig.savefig(figname, format = 'pdf')
    if(show==True):
        plt.show()
    else:
        plt.close(fig=fig)

def correct_bins(bins):
    diff = np.average(np.diff(bins))
    b = np.array(bins)
    return(b[:-1] + diff/2)

@timeit
def pdf_average_plot(Glist, **kwargs):

    fig = plt.figure()

    if(kwargs.get('xlabel')):
        plt.xlabel(kwargs['xlabel'])
    if(kwargs.get('ylabel')):
        plt.ylabel(kwargs['ylabel'])

    pdf_threshold = 1e-10

    for n in Glist[1:]:
        x, pdf = PDF_reader.reader_pdf(n)
        mask = pdf>pdf_threshold
        plt.hist(x[mask],weights=pdf[mask], bins=correct_bins(x[mask]), alpha=0.4)

    average_x, average_pdf = PDF_reader.reader_pdf(Glist[0])
    mask = average_pdf>pdf_threshold
    plt.hist(average_x[mask], weights=average_pdf[mask],
        bins=correct_bins(average_x[mask]), label='Average')

    plt.legend()

    if(kwargs.get('figname')):
        fig.savefig(kwargs['figname'], format = 'pdf')

    if(kwargs.get('show')):
        plt.show()
    plt.close(fig=fig)

if __name__ == '__main__':

    # plot_cloud(fringes_bp3p3, steps_bp3p3, params_bp3p3, pdf_bp3p3,errors = errors_bp3p3,
    #     radius = 20,  figname =  'figs/fit_cloud_bp3p3.pdf', show=False,
    #     fmt='ro', ms=2, capsize=1.5, elinewidth=1, markeredgewidth=0.5)
    #
    # ylims = [-800, -750]
    #
    #
    # plot_cloud(fringes_bp3p3, steps_bp3p3, params_bp3p3, pdf_bp3p3, errors = errors_bp3p3,
    #     xlims=xlims, ylims=ylims, figname = 'figs/fit_zoom_bp3p3.pdf', show=False,
    #     fmt='ro', ms=10, capsize=5, elinewidth=2, markeredgewidth=2)
    #
    # fig = plt.figure()
    # paraffs['bp3-p3'].plot(radius=0.09)
    # fig.savefig('figs/bp3p3_measure.pdf', format = 'pdf')
    # plt.close('')

    # find_point_plot([-10.02, -9.98], 'montecarlo/bp3p3_-10s50.txt', 'figs/3D_qbic_close.pdf', show=False)
    # find_point_plot([-13.80, -13.75], 'montecarlo/bp3p3_-13s50.txt', 'figs/3D_qbic_far.pdf', show=True)

    gamma_list = ['montecarlo/Gamma/average_gamma_new_G.txt',
        'montecarlo/Calibrate/qbic/ba1a1_gamma_G.txt',
        'montecarlo/Calibrate/qbic/ba1a2_gamma_G.txt',
        'montecarlo/Calibrate/qbic/ba1a3_gamma_G.txt',
        'montecarlo/Calibrate/qbic/ba2a1_gamma_G.txt',
        'montecarlo/Calibrate/qbic/ba2a2_gamma_G.txt',
        'montecarlo/Calibrate/qbic/ba2a3_gamma_G.txt']

    pdf_average_plot(gamma_list, xlabel='$\gamma$ [1]', ylabel='PDF [$1/\gamma$]',
        figname='figs/gamma.pdf')

    n_l_paraff_list = ['montecarlo/n_l_paraffin_average_G.txt',
    'montecarlo/Fit/fixed_alpha/bp1-p1_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp1-p2_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp1-p3_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp1-p4_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp3-p2_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp3-p3_n_l_G.txt',
    'montecarlo/Fit/fixed_alpha/bp3-p4_n_l_G.txt']

    pdf_average_plot(n_l_paraff_list, xlabel='$n_l$ [1]', ylabel='PDF [$1/n_l$]',
        figname='figs/n_l.pdf')
