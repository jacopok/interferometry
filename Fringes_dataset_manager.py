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

class dataset():
    
    def __init__(self, filename, flipped=False):
        self.filename = filename
        self.read_file(flipped)

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
            csv_reader = csv.reader(csv_file, delimiter=',')
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
        self.fringes_array = self.fringes_array - zero_fringe
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
        condition_nonzero = self.fringes_array != 0
        condition = condition_radius & condition_nonzero
        f = self.fringes_array[condition]
        s = self.step_array[condition]
        f = np.abs(f)
        
        parabola = lambda x, a, b, c: a*x**2 + b*x + c
        
        p, pcov = scipy.optimize.curve_fit(parabola, s, f)
        zero_fringe = p[2] - p[1]**2 / (4 * p[0])
        zero_step = - p[1] / (2 * p[0])
        return(zero_fringe, zero_step)