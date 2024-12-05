#!/bin/bash -x
#
#PJM -L "rscunit=rscunit_ft01,rscgrp=small-s1"
#PJM -L "node=4x5x3:torus,freq=2000"            # Specify 3 dimentions shape
#PJM -g ra000012
#PJM --mpi "proc=60"
#PJM --mpi "max-proc-per-node=1"
#PJM -L "elapse=00:40:00"
#PJM -s
#
unirandom_dir_stdout=./${PJM_MPI_PROC}proc_pingpong/
birandom_dir_stdout=./${PJM_MPI_PROC}proc_pingping/
# mpifcc -o a.out test_tofu_coordinate.c

date
# mpiexec -n ${PJM_MPI_PROC} -stdout-proc ${dir_stdout}/stdout -stderr-proc ${dir_stdout}/stderr ./a.out
# mpiexec -n ${PJM_MPI_PROC} -stdout-proc ${dir_stdout}/stdout -stderr-proc ${dir_stdout}/stderr IMB-P2P
mpiexec -n ${PJM_MPI_PROC} -stdout-proc ${unirandom_dir_stdout}/stdout -stderr-proc ${unirandom_dir_stdout}/stderr ./main PingPong
mpiexec -n ${PJM_MPI_PROC} -stdout-proc ${birandom_dir_stdout}/stdout -stderr-proc ${birandom_dir_stdout}/stderr ./main PingPing
date
