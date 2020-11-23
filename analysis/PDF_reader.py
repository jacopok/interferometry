#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed May  1 21:12:17 2019

@author: jacopo
"""

import csv
import numpy as np

def reader_mpdf(filename):

    with open(filename) as file:
        csv_reader = csv.reader(file, delimiter='\t')
        firstrow = next(csv_reader)
        #name = firstrow[0]
        # we do not need it but it is set
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

        # the indices in a np.array work such that the first one is the one that changes the slowest
        # while we want the first one to be the one that changes the fastest
        names, inf_limits, sup_limits, shape = \
            [np.flip(z) for z in (names, inf_limits, sup_limits, shape)]

        long_pdf = []
        for row in csv_reader:
            for number in row:
                if(number):
                    long_pdf.append(float(number))

        pdf = np.reshape(long_pdf, shape)

        x = []
        for n, s, il, sl in zip(names, shape, inf_limits, sup_limits):
            dx = np.abs((sl-il)/s)
            x.append(il + dx*(np.arange(s) + .5)) # Histogram bins should be centered
                                #on the midpoints
        x_coords = np.array(np.meshgrid(*x)).T.reshape(-1, len(x))
        return(x_coords, pdf.flatten(), names)


def reader_pdf(filename):

    with open(filename) as file:
        csv_reader = csv.reader(file, delimiter='\t')

        x = []
        pdf = []
        for row in csv_reader:
            x.append(row[0])
            pdf.append(row[1])

    return(np.array(x, dtype=np.float64), np.array(pdf, dtype=np.float64))

def reader_data(filename):

    with open(filename) as file:
        csv_reader = csv.reader(file, delimiter='\t')

        x=[]
        y=[]
        y_fw=[]
        for row in csv_reader:
            if(row[1]=='Triangular'):
                x.append(row[0])
                y.append(row[2])
                y_fw.append(row[3])
            else:
                raise NotImplementedError('This PDF type is not supported')

    return(np.array(x, dtype=np.float64),
        np.array(y, dtype=np.float64), np.array(y_fw, dtype=np.float64)/ np.sqrt(24))
        # sqrt(24) comes from the triangular distribution:
        # the fw is sqrt(24) times the stdev
