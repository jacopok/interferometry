#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  1 21:12:17 2019

@author: jacopo
"""

import csv
import numpy as np

def reader_pdf(filename):

    
    with open(filename) as file:
        csv_reader = csv.reader(file, delimiter='\t')
        firstrow = next(csv_reader)
        name = firstrow[0]
        number_of_pdfs = int(firstrow[1])
        pdfs = []
        for i in range(number_of_pdfs):
            row = next(csv_reader)
            pdfs.append(row)
        pdfs = np.array(pdfs)
        
        names = pdfs[:,0]
        inf_limits = np.array(pdfs[:,1], dtype=np.double)
        sup_limits = np.array(pdfs[:,2], dtype=np.double)
        shape = np.array(pdfs[:,3], dtype=np.int_)
        
        long_pdf = []
        for row in csv_reader:
            for number in row:
                if(number):
                    long_pdf.append(float(number))
        
        pdf = np.reshape(long_pdf, shape)
        
        x = {}
        for n, s, il, sl in zip(names, shape, inf_limits, sup_limits):
            x[n] = np.arange(il, sl, np.abs((sl-il)/s))
        
        return(x, pdf)