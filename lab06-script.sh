#!/bin/sh

module load intel/18.0.3

g++ -v  -g -Wall -fopenmp -o lab06 lab06.cpp

srun -t1 ./lab06 input64.ppm 4
