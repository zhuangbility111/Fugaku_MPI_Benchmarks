#include "benchmark.h"
#include "topology.h"
#include <mpi.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::string benchmark_name = "PingPong";
    int max_exponent = 22;   // Default max exponent for message size
    int repetitions = 1000;
    int warmup_rounds = 10;  // Default warmup rounds

    if (argc > 1) {
        benchmark_name = argv[1];
        if (benchmark_name != "PingPong" && benchmark_name != "PingPing" && benchmark_name != "ScatterGather") {
            std::cerr << "Invalid benchmark name: " << benchmark_name << std::endl;
            std::cerr << "Valid value: " << argv[0] << " [PingPong|PingPing|ScatterGather]" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (argc > 2) {
        max_exponent = atoi(argv[2]);
        if (max_exponent < 0 || max_exponent > 30) {
            std::cerr << "Invalid max exponent: " << max_exponent << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    int root = 0;
    std::vector<int> recv_ranks;
    for (int i = 0; i < size; i++) {
        recv_ranks.push_back(i);

    }

    if (benchmark_name == "ScatterGather") {
        if (argc < 5) {
            std::cerr << "Usage: " << argv[0] << " ScatterGather <max_exponent> <root> <recv_ranks>" << std::endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        root = atoi(argv[3]);
        if (root < 0) {
            std::cerr << "Invalid root: " << root << "\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        char *recv_ranks_str = argv[4];
        char *token = strtok(recv_ranks_str, ",");
        recv_ranks.clear();
        while (token != NULL) {
            int recv_rank = atoi(token);
            if (recv_rank < 0) {
                std::cerr << "Invalid recv rank: " << recv_rank << "\n";
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            recv_ranks.push_back(recv_rank);
            token = strtok(NULL, ",");
        }
    }

    // if (argc > 4) {
    //     repetitions = atoi(argv[3]);
    //     if (repetitions < 0) {
    //         std::cout << "Invalid repetitions: " << repetitions << "\n";
    //         MPI_Abort(MPI_COMM_WORLD, 1);
    //     }
    // }

    // if (argc > 5) {
    //     warmup_rounds = atoi(argv[4]);
    //     if (warmup_rounds < 0) {
    //         std::cout << "Invalid warmup rounds: " << warmup_rounds << "\n";
    //         MPI_Abort(MPI_COMM_WORLD, 1);
    //     }
    // }

    get_topology();

    Benchmark benchmark(max_exponent, warmup_rounds, repetitions, benchmark_name, root, recv_ranks);
    benchmark.run();

    MPI_Finalize();
    return 0;
}
