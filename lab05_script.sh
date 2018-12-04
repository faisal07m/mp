#!/bin/sh
module load intel/18.0.3
mpiicpc -g -Wall -o lab05 lab05.cpp
srun -t1  ./lab05 10000000 16
