#ifndef PINGPONG_H
#define PINGPONG_H

#include <mpi.h>
#include <string>

class PingPongBenchmark {
public:
    PingPongBenchmark(int max_exponent, int warmup_rounds);
    void run();

private:
    int max_exponent;       // 最大消息大小的指数
    int warmup_rounds;      // Warmup 轮数

    void warmup(int msg_size, int rank, int size);
    void benchmark(int msg_size, int repetitions, int rank, int size);
    int calculate_repetitions(int msg_size);
};

#endif // PINGPONG_H
