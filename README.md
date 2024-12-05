# Usage
## PingPong
```C++
mpirun -np 12 ./main PingPong [Exponent of Message size E(range: 1 - 30)] 
```

Message size is 2^`E`. The pattern of PingPong can be referred in https://www.intel.com/content/www/us/en/docs/mpi-library/user-guide-benchmarks/2021-2/pingpong-pingpongspecificsource-pingponganysource.html#PINGPONG-PATTERN

## PingPing
```C++
mpirun -np 12 ./main PingPing [Exponent of Message size E (range: 1 - 30)] 
```

Message size is 2^`E`. he pattern of PingPing can be referred in https://www.intel.com/content/www/us/en/docs/mpi-library/user-guide-benchmarks/2021-2/pingping-pingpingspecificsource-pingpinganysource.html#PINGPING-PATTERN

## ScatterGather (to check the bandwidth of virtual and phsycial shape)
```C++
mpirun -np 12 ./main ScatterGather [Exponent of Message size E (range: 1 - 30)] [rank of root process (only 1 root)] [rank of recv processes (multiple receivers, separate different receivers with commas)]
```

Message size is 2^`E`. This mode was designed by me to check the communication performance on Fugaku is determined by virtual topology or physical topology (According to the result, the answer is physical topology). Root uses MPI_Isend and MPI_Irecv to send and receive messages to other specified processes at the same time. Other processes receive messages from root by first calling MPI_Recv, and then calling MPI_Send to send messages back to root. It can be understood that root performs PingPong with multiple processes at the same time to verify the bandwidth when root communicates with multiple processes at the same time.