#!/bin/sh

module load intel/18.0.3

mpiicpc -g -Wall -o lab04 mmp.c
srun -t1 -A hpclco -n 4 ./lab04

