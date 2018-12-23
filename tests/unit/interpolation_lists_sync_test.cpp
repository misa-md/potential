//
// Created by genshen on 2018-12-23.
//

#include <gtest/gtest.h>
#include <interpolation_lists.hpp>

//@mpi test sync of broadcast.
TEST(it_lists_test_sync, it_lists_test) {
    int rank;
    const int root = 0;
    const int list_size = 2; // it must be known to all processors.

    InterpolationLists<int, InterpolationObject> itl(list_size); // capacity: 2
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == root) {
        // prepare data1
        itl.insert(1, InterpolationObject{});
        const int data_length = 10;
        double data[data_length] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        itl.get(1)->initInterpolationObject(data_length, 1.0, 1, data);
        // prepare data2
        itl.insert(2, InterpolationObject{});
        double data2[data_length] = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190};
        itl.get(2)->initInterpolationObject(data_length, 100, 10, data2);
    }

    // every processors known the size.
    itl.sync(list_size, root, rank, MPI_COMM_WORLD);

    EXPECT_EQ(itl.size(), 2); // test list length.
    // test first one.
    EXPECT_EQ(itl.get(1)->n, 10);
    EXPECT_EQ(itl.get(1)->x0, 1.0);
    EXPECT_EQ(itl.get(1)->invDx, 1.0 / 1); // invDx = 1.0 / dx;
    // test second one.
    EXPECT_EQ(itl.get(2)->n, 10);
    EXPECT_EQ(itl.get(2)->x0, 100);
    EXPECT_EQ(itl.get(2)->invDx, 1.0 / 10);
}

//@mpi test all sync of broadcast.
TEST(it_lists_test_sync_all, it_lists_test) {
    int rank;
    const int root = 0;
    const int list_size = 2; // it must be known to all processors.

    InterpolationLists<int, InterpolationObject> itl(list_size); // capacity: 2
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == root) {
        // prepare data1
        itl.insert(1, InterpolationObject{});
        const int data_length = 10;
        double data[data_length] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        itl.get(1)->initInterpolationObject(data_length, 1.0, 1, data);
        // prepare data2
        itl.insert(2, InterpolationObject{});
        double data2[data_length] = {100, 110, 120, 130, 140, 150, 160, 170, 180, 190};
        itl.get(2)->initInterpolationObject(data_length, 100, 10, data2);
    }

    // only root processor known the size.
    itl.sync(root, rank, MPI_COMM_WORLD);

    EXPECT_EQ(itl.size(), 2); // test list length.
    // test first one.
    EXPECT_EQ(itl.get(1)->n, 10);
    EXPECT_EQ(itl.get(1)->x0, 1.0);
    EXPECT_EQ(itl.get(1)->invDx, 1.0 / 1); // invDx = 1.0 / dx;
    // test second one.
    EXPECT_EQ(itl.get(2)->n, 10);
    EXPECT_EQ(itl.get(2)->x0, 100);
    EXPECT_EQ(itl.get(2)->invDx, 1.0 / 10);
}
