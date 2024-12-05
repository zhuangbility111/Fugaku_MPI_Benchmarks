#include "PingPong.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>

PingPongBenchmark::PingPongBenchmark(int max_exponent, int warmup_rounds)
    : max_exponent(max_exponent), warmup_rounds(warmup_rounds) {}

void PingPongBenchmark::run() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        std::cout << "#----------------------------------------------------------\n"
                  << "# Benchmarking PingPong\n"
                  << "# #processes = " << size << "\n"
                  << "#----------------------------------------------------------\n";
    }

    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes start together

    for (int exp = 0; exp <= max_exponent; ++exp) {
        int msg_size = 1 << exp; // Message size = 2^exp bytes
        int repetitions = calculate_repetitions(msg_size);

        // Warmup phase
        warmup(msg_size, rank, size);

        if (rank == 0) {

            std::cout << "----- Message size (Bytes): " << msg_size << " --- repetitions: " << repetitions << " -----\n";
            
            std::cout << std::setw(6) << "src"
                    << std::setw(6) << "dest"
                    << std::setw(12) << "t[usec]"
                    << std::setw(12) << "Mbytes/sec"
                    << std::setw(12) << "Msg/sec\n";
        }

        // Benchmark phase
        benchmark(msg_size, repetitions, rank, size);

        if (rank == 0) {
            std::cout << "----------------------------------------------------------\n";
        }
    }
}

void PingPongBenchmark::warmup(int msg_size, int rank, int size) {
    char *msg = new char[msg_size];
    memset(msg, 'a', msg_size);

    for (int dest = 1; dest < size; ++dest) {
        for (int i = 0; i < warmup_rounds; ++i) {
            if (rank == 0) {
                    MPI_Send(msg, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
                    MPI_Recv(msg, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == dest) {
                MPI_Recv(msg, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(msg, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
            MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        }
    }

    delete[] msg;
}

void PingPongBenchmark::benchmark(int msg_size, int repetitions, int rank, int size) {
    char *msg = new char[msg_size];
    memset(msg, 'a', msg_size);

    double total_time = 0.0;
    for (int dest = 1; dest < size; ++dest) {
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        if (rank == 0) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < repetitions; ++i) {
                MPI_Send(msg, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
                MPI_Recv(msg, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            double end_time = MPI_Wtime();
            total_time += (end_time - start_time) * 1e6; // Convert to microseconds

            double avg_time = total_time / repetitions / 2;
            double bandwidth = (msg_size / 1e6) / (avg_time / 1e6); // MB/sec
            double msg_rate = 1.0 / (avg_time / 1e6); // Messages/sec

            total_time = 0.0; // Reset total_time for next destination

            std::cout << std::setw(6) << "0"
                        << std::setw(6) << dest
                        << std::setw(12) << std::fixed << std::setprecision(2) << avg_time
                        << std::setw(12) << bandwidth
                        << std::setw(12) << msg_rate << "\n";
        } else if (rank == dest) {
            for (int i = 0; i < repetitions; ++i) {
                MPI_Recv(msg, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(msg, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    }

    delete[] msg;
}

int PingPongBenchmark::calculate_repetitions(int msg_size) {
	if (msg_size <= 8192)
		return 100000;
    const int base_reps = 51200; // Base repetitions for small message sizes
    return std::max(base_reps / (msg_size / 16384), 1);
}
