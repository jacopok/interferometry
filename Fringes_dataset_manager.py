#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import csv
import numpy as np
#import itertools
import matplotlib.pyplot as plt

def output(out_filename, step_array, fringes_array,
           firstline = "Step number, fringe number\n"):
    """
    Outputs to "out_filename" the data in csv format, with the specified
    first line.
    """
    
    data = np.stack((step_array, fringes_array), axis=-1)
    
    with open(out_filename, "w") as csv_file:
        csv_file.write(firstline)
        csv_writer = csv.writer(csv_file, delimiter = ',')
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
        self.negative_fringes_array = np.flipud(-f[f<0])
        self.positive_step_array = s[s>0]
        self.negative_step_array = np.flipud(-s[s<0])
        
    def output_split(self, out_filenames):
        """
        Takes a pair of strings as input, the filenames of the positive
        and of the negative fringes files respectively
        """
        output(out_filenames[0], self.positive_step_array, self.positive_fringes_array)
        output(out_filenames[1], self.negative_step_array, self.negative_fringes_array,
           firstline="Negative step number, negative fringe number\n")
    
        

    """
    def find_zero_fringe(self):
        
        This is not needed right now. It might be 
        
        
        for f in self.fringes_array[1:-1]:
            over_mask = self.fringes_array>f
            under_mask = self.fringes_array<f
            s_over = self.step_array[over_mask] 
            s_under = self.step_array[under_mask]
            f_over = self.fringes_array[over_mask] - f
            f_under = f - self.fringes_array[under_mask]
            print(f)
            error = 0
            counter = 0
            for f1, f2 in itertools.product(f_over, f_under):
                if(f1==f2):
                    error += np.abs(s_over[f_over==f1] - s_under[f_under==f1])
                    counter +=1
            print(error/counter)
    """