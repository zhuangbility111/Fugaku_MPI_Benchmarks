#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <mpi.h>
#include <string>
#include <vector>

class Benchmark {
public:
    Benchmark(int max_exponent, int warmup_rounds, int repetitions, std::string benchmark_name, int root, std::vector<int> recv_ranks);
    void run();

private:
    int max_exponent;       // 最大消息大小的指数
    int warmup_rounds;      // Warmup 轮数
    int repetitions;        // 测试轮数
    std::string benchmark_name; // Benchmark 名称
    int root;
    std::vector<int> recv_ranks;

    void calculate_repetitions(int msg_size);

    void print_header(int size);

    void PingPong(int msg_size, int rank, int size);
    void PingPing(int msg_size, int rank, int size);
    void ScatterGather(int msg_size, int rank, int size);
};

#endif // BENCHMARK_H
