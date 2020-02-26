//
// Created by genshen on 2020/2/26.
// The code of this file is copied from: https://gist.github.com/mdavezac/eb16de7e8fc08e522ff0d420516094f5
//
// The origin code is distributed under the MIT license
// The following demonstrates how to use google/benchmark with MPI enabled
// codes. There are three core aspects:
//
// 1. The time it takes to run a single iteration of a particular benchmark is
//    the maximum time across all processes. In a two process job, if process 0
//    takes 1 second to do its work, and process 1 takes 1.5 seconds, then 1.5
//    seconds is the time recorded for benchmarking purposes.
// 2. Only the root process is allowed to report. Other processes report via a
//    "NullReporter".
// 3. The main is modified to initialize and finalize MPI. It is also modified
// for the purpose of point 2 above.
//
// Note: The efficiency of MPI algorithm often depends on interweaving compute
// and communication. Depending on how it is applied, benchmarking, and
// especially micro-benchmarking, may not capture this aspect.
//

#include <mpi.h>
#include <benchmark/benchmark.h>
#include "eam_find_spline.cpp"

// This reporter does nothing.
// We can use it to disable output from all but the root process
class NullReporter : public ::benchmark::BenchmarkReporter {
public:
    NullReporter() = default;

    bool ReportContext(const Context &) override { return true; }

    void ReportRuns(const std::vector<Run> &) override {}

    void Finalize() override {}
};

// The main is rewritten to allow for MPI initializing and for selecting a
// reporter according to the process rank
int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    ::benchmark::Initialize(&argc, argv);
    if (rank == 0) {
        // root process will use a reporter from the usual set provided by
        // ::benchmark
        ::benchmark::RunSpecifiedBenchmarks();
    } else {
        // reporting from other processes is disabled by passing a custom reporter
        NullReporter null;
        ::benchmark::RunSpecifiedBenchmarks(&null);
    }
    MPI_Finalize();
    return 0;
}
