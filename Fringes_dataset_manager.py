#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import csv
import numpy as np
#import itertools
import matplotlib.pyplot as plt
import scipy.optimize

plt.style.use('seaborn')

def output(out_filename, fringes_array, step_array):
    """
    Outputs to "out_filename" the data in tab-separated format
    """
    
    data = np.stack((fringes_array, step_array), axis=-1)
    
    with open(out_filename, 'w') as csv_file:
        #csv_file.write(firstline)
        csv_writer = csv.writer(csv_file, delimiter = '\t')
        csv_writer.writerows(data)
        
def output_montecarlo(out_filename, fringes_array, angle_array,
                      step_error, gain_error):
    """
    Outputs to out_filename
    data in a format which is readable by the Montecarlo simulator
    found in ./montecarlo/
    Outputs in tab-separated rows:
        fringe number
        "Digital" (needed for the MC program to calculate the ditribution)
        angle
        step error (actually angle error: the constant part
                    of the error in the angle)
        gain error (percent error in the step-angle conversion, which
                    gets scaled as angles increase by the MC simulator)
        
    """
    
    n = len(fringes_array)
    step_error_array = np.ones(n) * step_error
    gain_error_array = np.ones(n) * gain_error
    digital_array = np.repeat("Digital", n)
    
    data = np.stack((fringes_array, digital_array,
                     angle_array, step_error_array, gain_error_array), axis=-1)

    with open(out_filename, "w+") as csv_file:
        csv_writer = csv.writer(csv_file, delimiter = '\t')
        csv_writer.writerows(data)
    
class dataset():
    """
    Contains a measurement set, from a certain angle to some other angle.
    The file in "filename" must contain the raw data,
    tab or space separated, in the order "fringe, step".
    """
    
    def __init__(self, filename=None, flipped=False):
        """
        Initialization of the data set.
        The conversion_factor, step_error and gain_error are hardcoded
        for now, since they just have to be 
        """
        
        if(filename):
            self.filename = filename
            self.read_file(flipped)
            #removes the file extension and the path to give just the name
            self.name = self.filename.split('.')[-2].split('/')[-1]
        self.conversion_factor = 42.6e-6
        self.step_error = 0.000213
        self.gain_error = 0.0047

    def read_file(self, flipped, order='same'):
        """
        The files must be tab- or space-separated
        with rows containing the fringe number, followed
        by the step number. If the order is inverted,
        the flipped=True option can be selected for the file to be read correctly.
        
        order can be 'auto', 'same' or 'reversed', depending on how
        you want the order of the fringes.
        'auto' checks if the steps are increasing with fringe number
        and adjusts accordingly
        """
        
        fringes_array = []
        step_array = []
        if(flipped==True):
            i_step=0
            i_fringe=1
        else:
            i_step=1
            i_fringe=0
            
        with open(self.filename) as file:
            firstrow = file.read(100)
            if(' ' in firstrow):
                delim = ' '
            elif('\t' in firstrow):
                delim = '\t'
            else:
                print('Delimiter error on file ' + self.filename)
                return(None)
        
        with open(self.filename) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter=delim)
            for row in csv_reader:
                if(len(row)>1):
                    step_array.append(row[i_step])
                    fringes_array.append(row[i_fringe])
        
        fringes_array = list(filter(None, fringes_array))
        step_array = list(filter(None, step_array))
        fringes_array = np.array(fringes_array, dtype='float')
        ufa = np.array(fringes_array, dtype='int')
        usa = np.array(step_array, dtype = 'float')

        if(order == 'auto'):
            if(usa[0] < usa[10]):
                order = 'same'
            else:
                order = 'flipped'        
        if(order == 'same'):
            pass
        elif(order == 'flipped'):
            ufa = -ufa
        else:
            return(None)
        
        self.uncentered_fringes_array = ufa
        self.uncentered_step_array = usa
        
    def filter_steps(self):
        """
        Removes duplicates of step-fringe couples
        """
        
        matrix = np.stack((self.step_array, self.fringes_array))
        uniques = np.unique(matrix, axis=1)
        self.step_array = uniques[0,:]
        self.fringes_array = uniques[1,:]
        
    def join(self, new_dataset):
        """
        Joins a new_dataset to the current one,
        adjusting the fringe numbers accordingly.
        Not needed anymore, probably.
        """
        self.uncentered_step_array = np.append(self.uncentered_step_array, new_dataset.uncentered_step_array)
        max_fringe = np.max(self.uncentered_fringes_array)
        new_dataset.uncentered_fringes_array += max_fringe + 1
        self.uncentered_fringes_array = np.append(self.uncentered_fringes_array, new_dataset.uncentered_fringes_array)
        
    def set_zero(self, zero_fringe=0):
        """
        Centers the data by setting the zero fringe to zero_fringe,
        and adjusting the step numbers accordingly (the zero step will then
        correspond to the zero fringe).
        If zero_fringe does not belong to the fringes_array, the two nearest fringes
        are interpreted to be the -1 and 1 fringes,
        (this is necessary when the inversion happens around a maximum).
        An average is then performed to find the zero step.
        
        The uncentered data is kept in the uncentered_* arrays,
        the centered data is stored in the fringes_array and the step_array
        """
        ufa = self.uncentered_fringes_array
        usa = self.uncentered_step_array
        if(zero_fringe in ufa):
            zero_step = usa[ufa == zero_fringe]
            self.fringes_array = ufa - zero_fringe
        else:
            f_bef = np.max(ufa[ufa<zero_fringe])
            f_aft = np.min(ufa[ufa>zero_fringe])
            self.fringes_array = ufa - f_bef
            self.fringes_array[self.fringes_array>=0] +=1
            zero_step = np.average((usa[ufa==f_bef], usa[ufa==f_aft]))
        self.step_array = usa - zero_step
        
    def set_zero_step(self, zero_step):
        """
        Subtracts the zero_step from the
        """
        self.step_array = self.step_array - zero_step
    
    def plot(self, radius=None, straight=False, label=None, residuals=False, **kwargs):
        """
        Plots the data using matplotlib.
        
        If radius is specified, only the data such that
        -radius <= step <= radius
        is plotted.
        
        If straight is set to True, the absolute value of the fringe numbers
        is taken.
        
        A label can be specified.
        """
        if(residuals==True):
            efa = self.fringes_array
            p = self.fit()
            tfa = self.offset_fringes(self.step_array, *p)
            fa = efa -tfa
        else:
            fa = self.fringes_array
        
        if(straight==True):
            fa = np.abs(fa)
            
        aa = self.angle_array
        
        mask=np.full(len(aa), True)
        if(radius!=None):
            mask=np.abs(self.angle_array)<radius
        
        plt.xlabel('Angle [rad]')
        plt.ylabel('Fringes [1]')
        plt.plot(aa[mask], fa[mask], **kwargs, label=label)
            
    def output(self, out_filename):
        """
        Outputs the centered data to out_filename
        in tab-separated rows,
        """
        
        output(out_filename, self.fringes_array, self.step_array)
    
    def output_mc(self, out_filename):
        output_montecarlo(out_filename, self.fringes_array,
                          self.angle_array, self.step_error, self.gain_error)
                
    def find_zero_fringe(self, fringes_radius, ignore_radius=1.5):
        """
        NOT USED ANYMORE
        
        Performs a parabolic fit of the data in a fringes_radius large radius
        around the origin, in order to find the precise position of the
        origin.
        
        NOT USED ANYMORE
        """
        condition_radius = np.abs(self.fringes_array) <= fringes_radius
        condition_nonzero = np.abs(self.fringes_array) > ignore_radius
        condition = condition_radius & condition_nonzero
        f = self.fringes_array[condition]
        s = self.step_array[condition]
        f = np.abs(f)
        
        parabola = lambda x, a, b, c: a*x**2 + b*x + c
        
        p, pcov = scipy.optimize.curve_fit(parabola, s, f)
        zero_fringe = p[2] - p[1]**2 / (4 * p[0])
        zero_step = - p[1] / (2 * p[0])
        return(zero_fringe, zero_step)

    def find_zero_fringe_asymm(self, fringes_radius, ignore_radius=1.5):
        """
        NOT USED ANYMORE
        
        Performs a parabolic fit of the data in a fringes_radius large radius
        around the origin, in order to find the precise position of the
        origin.
        
        NOT USED ANYMORE
        """

        condition_radius = np.abs(self.fringes_array) <= fringes_radius
        condition_nonzero = np.abs(self.fringes_array) > ignore_radius
        condition = condition_radius & condition_nonzero
        f = self.fringes_array[condition]
        s = self.step_array[condition]
        
        parabola = lambda step, a, zero_step, fringe_offset, missed_fringes: \
            (a*np.abs(step - zero_step)*(step - zero_step) + fringe_offset
             + (1 + np.sign(step - zero_step))/2*missed_fringes)
        
        p, pcov = scipy.optimize.curve_fit(parabola, s, f)
        return(p)
        
    def calculate_angles(self):
        """
        Fills self.angle_array by converting self.step_array
        """
        
        self.angle_array = self.conversion_factor * self.step_array

    def fringe_linearized_step(self, step):
        """
        Finds the fringe number which would correspond to
        the selected step, using a linear approximation.
        """
        
        sa = self.step_array
        
        if(step<=np.min(sa)):
            before_step = after_step = np.min(sa)
        elif(step>=np.max(sa)):
            before_step = after_step = np.max(sa)
        else:        
            before_step = np.max(sa[sa<=step])
            after_step = np.min(sa[sa>=step])
        
        diff = after_step-before_step
        f1 = self.fringes_array[sa==before_step]
        f2 = self.fringes_array[sa==after_step]
        if(f1==f2):
            fringe = f2
        else:
            fringe = (f2*(step-before_step) + f1*(after_step-step))/diff
        return(fringe)
        
    def analyze_fine(self):
        """
        Find the zero step by fitting the data to the model with self.fit
        Sets the zero step, then calculates the new angles.
        """
        zero_step, zero_fringe, gamma, index = self.fit()
        self.set_zero_step(zero_step)
        self.calculate_angles()
        
    def fringes_th(self, step, gamma, index):
        '''
        Number of fringes observed between 0 and theta due to a
        "thickness" thick layer of medium (with gamma = wavelength / (2*thickness)) 
        of refractive index "index".
        '''
        theta = step * self.conversion_factor
        
        nu = 1 / gamma * ( 1 - index - np.cos(theta) 
                                           + np.sqrt(index**2 - np.sin(theta)**2))
        return(nu)
        
    def offset_fringes(self, step, zero_step, zero_fringe, gamma, index):
        """
        The model in self.fringes_th, shifted such that the
        zero step is at zero_step, with a fringe number offset
        to account for the zero not being at a whole number of
        fringes, and with the fringes numbers flipped for
        negative (step-zero_step).
        
        Given a step, returns the corresponding model fringe.
        """
        
        return(np.sign(step - zero_step) * 
               ( self.fringes_th(step-zero_step, gamma, index) + zero_fringe))
    
    def fit(self, fringes_radius=1000, ignore_radius=0,
                     p0=(0,0,2.9e-5,1.3),
                     bounds=([-4000, -1, 5e-6, 1],
                              [4000, 1, 2e-4, 3])):
        """
        Fits the model in self.offset_fringes
        
        Returns:
            zero_step
            zero_fringe
            gamma = lambda/(2d)
            index of refraction of the medium
            
        The initial parameter values can be set as the array "p0",
        the bounds can be set as a tuple of array "bounds"
        """
        condition_radius = np.abs(self.fringes_array) <= fringes_radius
        condition_nonzero = np.abs(self.fringes_array) > ignore_radius
        condition = condition_radius & condition_nonzero
        f = self.fringes_array[condition]
        s = self.step_array[condition]
            
        p, pcov = scipy.optimize.curve_fit(self.offset_fringes, s, f,
                                           p0=p0, bounds=bounds)
        return(p)
        

    
class measure():
    """
    A class containing a two datasets: background and
    the gross signal from both the background and the liquid.
    """
    
    def __init__(self, background, gross_signal):
        
        self.background = background
        self.gross_signal = gross_signal
        self.background.filter_steps()
        self.gross_signal.filter_steps()
        self.background.calculate_angles()
        self.gross_signal.calculate_angles()
        self.name = ''
        
    def subtract_background(self, use_data=True):
        """
        For each step of the signal data, subtracts the corresponding 
        background fringe;
        if use_data is True this is found by linearly interpolating
        between the closest steps,
        if use_data is False this is found by fitting the model to the
        background data, and subtracting the model value to each signal datum.
        """
        
        import copy
        self.signal = copy.deepcopy(self.gross_signal)
        zs, zf, g, i = self.background.fit()
        bsr = (np.min(self.background.step_array), np.max(self.background.step_array))
        for step in self.gross_signal.step_array:
            if(use_data==True):
                bkg_fringe = self.background.fringe_linearized_step(step)
            else:
                bkg_fringe = self.background.offset_fringes(step, zs, zf, g, i)
            self.signal.fringes_array[self.gross_signal.step_array==step] -= bkg_fringe
        ssa = self.signal.step_array
        mask1 = ssa>bsr[0]
        mask2 = ssa<bsr[1]
        mask = mask1 & mask2
        self.signal.fringes_array = self.signal.fringes_array[mask]
        self.signal.step_array = self.signal.step_array[mask]
        self.signal.calculate_angles()

    def plot(self, **kwargs):
        name = self.name
        self.background.plot(**kwargs, label='Background: ' + name)
        self.signal.plot(**kwargs, label='Net signal: ' + name)
        self.gross_signal.plot(**kwargs, label='Gross signal: ' + name)
        plt.legend()