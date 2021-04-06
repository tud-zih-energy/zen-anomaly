Supplimentary stuff for the "Investigating the Cause and Effect of an AMD Zen Energy Management Anomaly"

# Contents

## measurement.py

Measurement automation code.

This currently uses metricQ as a means to get per-node power readings, but it shouldn't be a problem to replace it with another tool

This requires the powerread and compute programs which can be compiled using the provided Makefile.
## powerread.c

This uses lm_sensors[1] and x86_adapt [2] to get per-package and
per-core voltage values

## workload.cpp

A simple workload that performs a vector multiply-and-add loop for 10 seconds. 

Usage: ./compute [cpuid] [cpuid] [cpuid]
For example ./compute 0 1 4 - Three threads pinned to cores 0,1,4

## charts/reference.ipynb

Jupyter notebook used to generate Figure 2

## charts/percentages.ipynb

Jupyter notebook used to generate Figure 3

# results/

Contains the measurement data used to generate the figures


[1] https://github.com/lm-sensors/lm-sensors
[2] https://github.com/tud-zih-energy/x86_adapt
