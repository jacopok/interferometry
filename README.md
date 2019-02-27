Code for the analysis of the data from the GIMLI interferometer.

To center the data in a tab-separated file, use:

`python center_fringes.py <filename.txt> <zero_fringe_order>`

where `filename.txt` is the file where the data are stored,
whose first line is ignored; `zero_fringe_order` is an integer
corresponding to the order of the fringe which is manually identified 
to be of order 0.

This creates a file named `filename_centered.txt` with the same formatting.

Also, the tab-separated file `filename_centered_mc.txt` is created, which is ready for MonteCarlo simulations.
