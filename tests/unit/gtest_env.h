//
// Created by {{.Application.Author}} on {{.Application.Date}}.
//

#ifndef _GTEST_ENV_H
#define _GTEST_ENV_H
#include <gtest/gtest.h>
#include "test_config.h"

#  ifdef TEST_MPI_ENABLE_FLAG
#    include "mpi.h"

class MPIEnvironment : public ::testing::Environment {
public:
    virtual void SetUp() {
        char **argv;
        int argc = 0;
        int mpiError = MPI_Init(&argc, &argv);
        ASSERT_FALSE(mpiError);
    }

    virtual void TearDown() {
        int mpiError = MPI_Finalize();
        ASSERT_FALSE(mpiError);
    }

    virtual ~MPIEnvironment() {}
};
#  endif  // end TEST_MPI_ENABLE_FLAG

// see https://github.com/google/googletest/issues/822 for more information.
// main function for adapt mpi environment
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
#ifdef TEST_MPI_ENABLE_FLAG
    ::testing::AddGlobalTestEnvironment(new MPIEnvironment);
#endif  // end TEST_MPI_ENABLE_FLAG
    return RUN_ALL_TESTS();
}
#endif // _GTEST_ENV_H
