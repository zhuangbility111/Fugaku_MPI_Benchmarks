#include "benchmark.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <cstring>

Benchmark::Benchmark(int max_exponent, int warmup_rounds, int repetitions, std::string benchmark_name, int root, std::vector<int> recv_ranks)
    : max_exponent(max_exponent), warmup_rounds(warmup_rounds), repetitions(repetitions), benchmark_name(benchmark_name), root(root), recv_ranks(recv_ranks) {}

void Benchmark::print_header(int size) {
    std::cout << "#----------------------------------------------------------\n"
                << "# Benchmarking " << benchmark_name << "\n"
                << "# #processes = " << size << "\n";
    
    std::cout << "# root = " << root << "\n";
    std::cout << "# recv_ranks = ";
    for (int i = 0; i < recv_ranks.size(); ++i) {
        std::cout << recv_ranks[i] << " ";
    }

    std::cout << "\n#----------------------------------------------------------\n";
}

void Benchmark::PingPong(int msg_size, int rank, int size) {
    char *send_buf = new char[msg_size];
    char *recv_buf = new char[msg_size];
    memset(send_buf, 'b', msg_size);
    
    // Warmup phase
    for (int dest = 1; dest < size; ++dest) {
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        for (int i = 0; i < warmup_rounds; ++i) {
            if (rank == 0) {
                MPI_Send(send_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else if (rank == dest) {
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    }

    // Benchmark phase
    double total_time = 0.0;
    for (int dest = 1; dest < size; ++dest) {
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        if (rank == 0) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < repetitions; ++i) {
                MPI_Send(send_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
            double end_time = MPI_Wtime();
            total_time += (end_time - start_time) * 1e6; // Convert to microseconds

            double total_time_dest = 0.0;
            MPI_Recv(&total_time_dest, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (total_time_dest > total_time) {
                total_time = total_time_dest;
            }

            double avg_time = total_time / 2.0 / repetitions;
            double bandwidth = (msg_size / 1e6) / (avg_time / 1e6); // MB/sec
            double msg_rate = 1.0 / (avg_time / 1e6); // Messages/sec

            total_time = 0.0; // Reset total_time for next destination

            std::cout << std::setw(6) << "0"
                        << std::setw(6) << dest
                        << std::setw(12) << std::fixed << std::setprecision(2) << avg_time
                        << std::setw(12) << bandwidth
                        << std::setw(12) << msg_rate << "\n";
        } else if (rank == dest) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < repetitions; ++i) {
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(send_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
            double end_time = MPI_Wtime();
            total_time += (end_time - start_time) * 1e6; // Convert to microseconds

            // send total_time to rank 0
            MPI_Send(&total_time, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            total_time = 0.0;
        }
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    }

    delete[] send_buf;
    delete[] recv_buf;
}

void Benchmark::PingPing(int msg_size, int rank, int size) {
    MPI_Status stat;
    MPI_Request request;

    char *send_buf = new char[msg_size];
    char *recv_buf = new char[msg_size];
    memset(send_buf, 'a', msg_size);

    // Warmup phase
    for (int dest = 1; dest < size; ++dest) {
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        for (int i = 0; i < warmup_rounds; ++i) {
            if (rank == 0) {
                MPI_Isend(send_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, &request);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, &stat);
                MPI_Wait(&request, &stat);
            } else if (rank == dest) {
                MPI_Isend(send_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
                MPI_Wait(&request, &stat);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    }

    // Benchmark phase
    double total_time = 0.0;
    for (int dest = 1; dest < size; ++dest) {
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
        if (rank == 0) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < repetitions; ++i) {
                MPI_Isend(send_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, &request);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, dest, 0, MPI_COMM_WORLD, &stat);
                MPI_Wait(&request, &stat);
            }
            double end_time = MPI_Wtime();
            total_time += (end_time - start_time) * 1e6; // Convert to microseconds

            double total_time_dest = 0.0;
            MPI_Recv(&total_time_dest, 1, MPI_DOUBLE, dest, 0, MPI_COMM_WORLD, &stat);
            if (total_time_dest > total_time) {
                total_time = total_time_dest;
            }

            double avg_time = total_time / repetitions;
            double bandwidth = (msg_size / 1e6) / (avg_time / 1e6); // MB/sec
            double msg_rate = 1.0 / (avg_time / 1e6); // Messages/sec

            total_time = 0.0; // Reset total_time for next destination

            std::cout << std::setw(6) << "0"
                        << std::setw(6) << dest
                        << std::setw(12) << std::fixed << std::setprecision(2) << avg_time
                        << std::setw(12) << bandwidth
                        << std::setw(12) << msg_rate << "\n";
        } else if (rank == dest) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < repetitions; ++i) {
                MPI_Isend(send_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &request);
                MPI_Recv(recv_buf, msg_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
                MPI_Wait(&request, &stat);
            }
            double end_time = MPI_Wtime();
            total_time += (end_time - start_time) * 1e6; // Convert to microseconds

            // send total_time to rank 0
            MPI_Send(&total_time, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            total_time = 0.0;
        }
        MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    }

    delete[] send_buf;
    delete[] recv_buf;
}

void Benchmark::ScatterGather(int msg_size, int rank, int size) {
    int num_recv_ranks = recv_ranks.size();
    char *send_buf = nullptr;
    char *recv_buf = nullptr;

    MPI_Request* send_request = nullptr;
    MPI_Request* recv_request = nullptr;
    
    if (rank == root) {
        size_t send_size = (size_t)msg_size * num_recv_ranks;
        size_t recv_size = (size_t)msg_size * num_recv_ranks;

        send_buf = new char[send_size];
        recv_buf = new char[recv_size];

        memset(send_buf, 'a', send_size);

        send_request = new MPI_Request[num_recv_ranks];
        recv_request = new MPI_Request[num_recv_ranks];

    } else {
        send_buf = new char[msg_size];
        recv_buf = new char[msg_size];
        memset(send_buf, 'b', msg_size);
    }

    // Warmup phase
    for (int k = 0; k < warmup_rounds; ++k) {
        if (rank == root) {
            for (int i = 0; i < num_recv_ranks; ++i) {
                MPI_Isend(send_buf + i * msg_size, msg_size, MPI_CHAR, recv_ranks[i], 0, MPI_COMM_WORLD, send_request + i);
                MPI_Irecv(recv_buf + i * msg_size, msg_size, MPI_CHAR, recv_ranks[i], 0, MPI_COMM_WORLD, recv_request + i);
            }
        } else {
            for (int i = 0; i < num_recv_ranks; ++i) {
                if (rank == recv_ranks[i]) {
                    MPI_Recv(recv_buf, msg_size, MPI_CHAR, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Send(recv_buf, msg_size, MPI_CHAR, root, 0, MPI_COMM_WORLD);
                }
            }
        }
    }

    // Benchmark phase
    double min_total_time = 1e9;
    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding
    for (int k = 0; k < repetitions; ++k) {
        if (rank == root) {
            double start_time = MPI_Wtime();
            for (int i = 0; i < num_recv_ranks; ++i) {
                MPI_Isend(send_buf + i * msg_size, msg_size, MPI_CHAR, recv_ranks[i], 0, MPI_COMM_WORLD, send_request + i);
                MPI_Irecv(recv_buf + i * msg_size, msg_size, MPI_CHAR, recv_ranks[i], 0, MPI_COMM_WORLD, recv_request + i);
            }
            
            for (int i = 0; i < num_recv_ranks; ++i) {
                MPI_Wait(send_request + i, MPI_STATUS_IGNORE);
                MPI_Wait(recv_request + i, MPI_STATUS_IGNORE);
            }
            double end_time = MPI_Wtime();
            double total_time = (end_time - start_time) * 1e6; // Convert to microseconds
            if (total_time < min_total_time) { // Find the minimum total time of all repetitions
                min_total_time = total_time;
            }
        } else {
            for (int i = 0; i < num_recv_ranks; ++i) {
                if (rank == recv_ranks[i]) {
                    MPI_Send(recv_buf, msg_size, MPI_CHAR, root, 0, MPI_COMM_WORLD);
                    MPI_Recv(recv_buf, msg_size, MPI_CHAR, root, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD); // Ensure all processes finish before proceeding

    if (rank == root) {
        double bandwidth = (2.0 * msg_size * num_recv_ranks / 1e6) / (min_total_time / 1e6); // MB/sec
        double msg_rate = (2.0 * num_recv_ranks) / (min_total_time / 1e6); // Messages/sec

        for (int i = 0; i < num_recv_ranks; ++i) {
            std::cout << std::setw(6) << root
                        << std::setw(6) << recv_ranks[i]
                        << std::setw(12) << std::fixed << std::setprecision(2) << min_total_time
                        << std::setw(12) << bandwidth
                        << std::setw(12) << msg_rate << "\n";
        }

        delete[] send_buf;
        delete[] recv_buf;
        delete[] send_request;
        delete[] recv_request;
    } else {
        delete[] send_buf;
        delete[] recv_buf;
    }

}

void Benchmark::run() {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        print_header(size);
    }

    for (int exp = 0; exp <= max_exponent; ++exp) {
        int msg_size = 1 << exp; // Message size = 2^exp bytes
        calculate_repetitions(msg_size);

        if (rank == 0) {
            std::cout << "----- Message size (Bytes): " << msg_size << " --- repetitions: " << repetitions << " -----\n";
            
            std::cout << std::setw(6) << "src"
                    << std::setw(6)  << "dest"
                    << std::setw(12) << "t[usec]"
                    << std::setw(12) << "Mbytes/sec"
                    << std::setw(12) << "Msg/sec\n";
        }

        if (benchmark_name == "PingPong") {
            PingPong(msg_size, rank, size);
        } else if (benchmark_name == "PingPing") {
            PingPing(msg_size, rank, size);
        } else if (benchmark_name == "ScatterGather") {
            ScatterGather(msg_size, rank, size);
        }

        if (rank == 0) {
            std::cout << "----------------------------------------------------------\n";
        }
    }
}

void Benchmark::calculate_repetitions(int msg_size) {
	if (msg_size <= 8192)
		repetitions = 100000;
	else {
    	const int base_reps = 51200; // Base repetitions for small message sizes
    	repetitions = std::max(base_reps / (msg_size / 16384), 1);
	}
}
