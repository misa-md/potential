//
// Created by genshen on 2018/12/22.
//

#include <gtest/gtest.h>
#include <data_structure/array_map.hpp>

TEST(arr_map_insert_return, arr_map_test) {
    ArrayMap<int, int> map(2);
    int index = map.insert(1, 2);
    EXPECT_EQ(index, 0);
    index = map.insert(1, 2);
    EXPECT_EQ(index, 0); // not change
}

TEST(arr_map_insert_capacity, arr_map_test) {
    ArrayMap<int, int> map(0);
    int index = map.insert(1, 2);
    int except = ArrayMap<int, int>::INDEX_UN_LIMITED;
    EXPECT_EQ(index, except);
}

TEST(arr_map_insert_capacity2, arr_map_test) {
    ArrayMap<int, int> map(1);
    int index = map.insert(1, 2);
    EXPECT_EQ(index, 0);

    index = map.insert(2, 2);
    int except = ArrayMap<int, int>::INDEX_UN_LIMITED;
    EXPECT_EQ(index, except);
}

// test size.
TEST(arr_map_insert_size, arr_map_test) {
    ArrayMap<int, int> map(2);
    EXPECT_EQ(map.size(), 0);
    map.insert(1, 2);
    map.insert(2, 4);
    EXPECT_EQ(map.size(), 2);
    map.insert(3, 8); // not inserted.
    EXPECT_EQ(map.size(), 2);
}

// test get
TEST(arr_map_insert_get, arr_map_test) {
    int except_un_limit = ArrayMap<int, int>::INDEX_UN_LIMITED;

    ArrayMap<int, int> map(3);
    map.insert(1, 2);
    map.insert(2, 4);
    map.insert(3, 8);

    int *data = map.get(4);
    EXPECT_EQ(data, nullptr);

    data = map.get(3);
    EXPECT_EQ(*data, 8);

    // change data
    *data = 6;
    int *data2 = map.get(3);
    EXPECT_EQ(*data2, 6);
}
