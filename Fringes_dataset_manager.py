#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import csv
import numpy as np
#import itertools
import matplotlib.pyplot as plt
import scipy.optimize

def output(out_filename, step_array, fringes_array,
           firstline = "Step number, fringe number\n"):
    """
    Outputs to "out_filename" the data in csv format, with the specified
    first line.
    """
    
    data = np.stack((step_array, fringes_array), axis=-1)
    
    with open(out_filename, "w") as csv_file:
        csv_file.write(firstline)
        csv_writer = csv.writer(csv_file, delimiter = '\t')
        csv_writer.writerows(data)
        
def output_montecarlo(out_filename, step_array, fringes_array, angle_array,
                      step_error, gain_error):
    
    step_error_array = np.ones(len(step_array)) * step_error
    gain_error_array = np.ones(len(step_array)) * gain_error
    digital_array = np.repeat("Digital", len(step_array))
    
    data = np.stack((fringes_array, digital_array,
                     angle_array, step_error_array, gain_error_array), axis=-1)

    with open(out_filename, "w") as csv_file:
        csv_writer = csv.writer(csv_file, delimiter = '\t')
        csv_writer.writerows(data)
    
class dataset():
    
    def __init__(self, filename, flipped=False):
        """
        Initialization of the data set.
        The conversion_factor, step_error and gain_error are hardcoded
        for now, since they just have to be 
        """
        
        self.filename = filename
        self.read_file(flipped)
        self.conversion_factor = 42.6 * 10**(-6)
        self.step_error = 0.000213
        self.gain_error = 0.0173

    def read_file(self, flipped):
        """
        Works with files having exactly one starting line of comment.
        The files must be CSV with rows containing the step number, followed
        by the fringe number.
        """
        
        fringes_array = []
        step_array = []
        if(flipped==False):
            i_step=0
            i_fringe=1
        else:
            i_step=1
            i_fringe=0
        with open(self.filename) as csv_file:
            csv_reader = csv.reader(csv_file, delimiter='\t')
            next(csv_reader)
            for row in csv_reader:
                step_array.append(row[i_step])
                fringes_array.append(row[i_fringe])
        self.uncentered_fringes_array = np.array(fringes_array, dtype="int")
        self.uncentered_step_array = np.array(step_array, dtype="int")
        
    def set_zero(self, zero_fringe):
        """
        The zero_fringe must belong to the uncentered_fringes_array
        """
        zero_fringe = int(zero_fringe)
        zero_step = self.uncentered_step_array[self.uncentered_fringes_array == zero_fringe]
        self.fringes_array = self.uncentered_fringes_array - zero_fringe
        self.step_array = self.uncentered_step_array - zero_step
    
    def set_zero_fine(self, zero_fringe, zero_step = None):
        """
        The zero_fringe must belong to the uncentered_fringes_array
        """
        if(zero_step==None):
            zero_step = self.uncentered_step_array[self.uncentered_fringes_array == zero_fringe]
        self.fringes_array = np.sign(self.fringes_array) * \
            (np.abs(self.fringes_array) - zero_fringe)
        self.step_array = self.step_array - zero_step
    
    def plot(self, uncentered=False):
        if(uncentered==True):
            plt.scatter(self.uncentered_step_array, self.uncentered_fringes_array)
        else:
            plt.scatter(self.step_array, self.fringes_array)
            
    def split(self):
        """
        Must be called when the zero has already been set.
        """
        f = self.fringes_array
        s = self.step_array
        self.positive_fringes_array = f[f>0]
        self.negative_fringes_array = f[f<0]
        self.positive_step_array = s[s>0]
        self.negative_step_array = s[s<0]
        
    def output_centered(self, out_filename):
        output(out_filename, self.step_array, self.fringes_array)
    
    def output_centered_mc(self, out_filename):
        output_montecarlo(out_filename, self.step_array, self.fringes_array,
                          self.angle_array, self.step_error, self.gain_error)
                
    def output_split(self, out_filenames):
        """
        Takes a pair of strings as input, the filenames of the positive
        and of the negative fringes files respectively
        """
        output(out_filenames[0], self.positive_step_array, self.positive_fringes_array)
        output(out_filenames[1], self.negative_step_array, self.negative_fringes_array,
           firstline="Negative step number, negative fringe number\n")
    
    def find_zero_fringe(self, fringes_radius):
        """
        Performs a parabolic fit of the data in a fringes_radius large radius
        around the origin, in order to find the precise position of the
        origin.
        """
        condition_radius = np.abs(self.fringes_array) <= fringes_radius
        condition_nonzero = np.abs(self.fringes_array) > 1.5
        condition = condition_radius & condition_nonzero
        f = self.fringes_array[condition]
        s = self.step_array[condition]
        f = np.abs(f)
        
        parabola = lambda x, a, b, c: a*x**2 + b*x + c
        
        p, pcov = scipy.optimize.curve_fit(parabola, s, f)
        zero_fringe = p[2] - p[1]**2 / (4 * p[0])
        zero_step = - p[1] / (2 * p[0])
        return(zero_fringe, zero_step)
        
    def calculate_angles(self):
        self.angle_array = self.conversion_factor * self.step_array

    def fringe_linearized_step(self, step):
        """
        Finds the fringe number which would correspond to
        the selected step, using a linear approximation.
        """
        
        if(step<np.min(self.step_array)):
            before_step = after_step = np.min(self.step_array)
        elif(step>np.max(self.step_array)):
            before_step = after_step = np.max(self.step_array)
        else:        
            before_step = np.max(self.step_array[self.step_array<=step])
            after_step = np.min(self.step_array[self.step_array>=step])
        
        diff = after_step-before_step
        f1 = self.fringes_array[self.step_array==before_step]
        f2 = self.fringes_array[self.step_array==after_step]
        if(f1==f2):
            fringe = f2
        else:
            fringe = (f1*(step-before_step) + f2*(after_step-step))/diff
        print(before_step)
        print(after_step)
        return(fringe)
        
class measure():
    
    def __init__(self, background, signal):
        self.background = background
        self.signal = signal
    
    def subtract_background(self):
        for step in self.signal.step_array:
            bkg_fringe = self.background.fringe_linearized_step(step)
            self.signal.fringes_array[self.signal.step_array==step] -= bkg_fringe