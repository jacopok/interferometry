Code for the analysis of the data from the GIMLI interferometer.

-----------

Rough analysis without much explaination can be found in `Interferometry.ipynb` and in `rough_calculations.ipynb`.

------------

The figures are generated from `figure_generator.py`, which
contains lots of hardcoded file paths and does not have lots of comments for now.

It saves the figures to `figs/`; it it is not generating some figure, check at
the bottom of the file inside the `if __name__ == __main__` statement, and
uncomment what's needed.

-----------

The data preprocessing is done by the python code in the main directory,
the dataset classes are defined in `Fringes_dataset_manager.py`, which is called
by other utility scripts like `initialization.py` and `process_data.py`.

-----------

The data fitting is done by the c++ code in `montecarlo/`.
To compile the MonteCarlo sources, do:

```
cd montecarlo
source compileSources
```

-----------
