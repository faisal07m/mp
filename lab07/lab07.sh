#!/bin/sh

module load intel/18.0.3

g++ -g -O -fopenmp -o lab07 lab07.cpp

OMP_NUM_THREADS=20 srun -t1 ./lab07 3000
