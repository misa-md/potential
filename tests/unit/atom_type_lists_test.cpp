//
// Created by genshen on 2018-12-22.
//

#include <container/atom_type_lists.h>
#include <gtest/gtest.h>

// insert test
TEST(test_atom_type_lists_add, atom_type_lists_test) {
    AtomPropsList list;
    auto key1 = list.addAtomProp(26, "Fe", 55.845, 1.0, 2.0);
    auto key2 = list.addAtomProp(26, "Fe", 55.845, 2.0, 4.0);

    EXPECT_EQ(key1, 26);
    EXPECT_EQ(key2, AtomPropsList::KeyPropExists);
    EXPECT_EQ(list.size(), 1);
}

TEST(test_atom_type_lists_add_2, atom_type_lists_test) {
    AtomPropsList list;
    auto key1 = list.addAtomProp(26, "Fe", 55.845, 1.0, 2.0);
    auto key2 = list.addAtomProp(29, "Cu", 63.546, 2.0, 4.0);

    EXPECT_EQ(key1, 26);
    EXPECT_EQ(key2, 29);
    EXPECT_EQ(list.size(), 2);
}

// test getIndex
TEST(test_atom_type_lists_index, atom_type_lists_test) {
    AtomPropsList list;
    list.addAtomProp(26, "Fe", 55.845, 1.0, 2.0);
    list.addAtomProp(29, "Cu", 63.546, 2.0, 4.0);

    EXPECT_EQ(list.getIndex(26), 0);
    EXPECT_EQ(list.getIndex(29), 1);
    EXPECT_EQ(list.getIndex(32), AtomPropsList::KeyPropNotFound);
    EXPECT_EQ(list.findPropByNo(29).weight, 63.546);
}

//@mpi test sync
TEST(test_atom_type_lists_sync, atom_type_lists_test) {
    AtomPropsList list;
    int rank;
    const int root = 0;
    const int list_size = 2; // it must be known to all processors.
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank == root) {
        list.addAtomProp(26, "Fe", 55.845, 1.0, 2.0);
        list.addAtomProp(29, "Cu", 63.546, 2.0, 4.0);
    }
    list.sync(root, rank, MPI_COMM_WORLD, list_size);

    EXPECT_EQ(list.size(), 2);
    EXPECT_EQ(list.getIndex(26), 0);
    EXPECT_EQ(list.findPropByNo(29).weight, 63.546);
}