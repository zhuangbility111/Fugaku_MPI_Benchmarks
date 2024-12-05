#include "benchmark.h"
#include "topology.h"
#include <mpi.h>
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    std::string benchmark_name = "PingPong";
    int max_exponent = 22;   // Default max exponent for message size
    int repetitions = 1000;
    int warmup_rounds = 10;  // Default warmup rounds

    if (argc > 1) {
        benchmark_name = argv[1];
        if (benchmark_name != "PingPong" && benchmark_name != "PingPing") {
            std::cout << "Invalid benchmark name: " << benchmark_name << "\n";
            std::cout << "Valid value: " << argv[0] << " [PingPong|PingPing]\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (argc > 2) {
        max_exponent = atoi(argv[2]);
        if (max_exponent < 0 || max_exponent > 30) {
            std::cout << "Invalid max exponent: " << max_exponent << "\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (argc > 3) {
        repetitions = atoi(argv[3]);
        if (repetitions < 0) {
            std::cout << "Invalid repetitions: " << repetitions << "\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    if (argc > 4) {
        warmup_rounds = atoi(argv[4]);
        if (warmup_rounds < 0) {
            std::cout << "Invalid warmup rounds: " << warmup_rounds << "\n";
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    get_topology();

    Benchmark benchmark(max_exponent, warmup_rounds, repetitions, benchmark_name);
    benchmark.run();

    MPI_Finalize();
    return 0;
}
