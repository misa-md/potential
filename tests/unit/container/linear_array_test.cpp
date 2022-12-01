//
// Created by genshen on 2022/9/24.
//

#include <gtest/gtest.h>

#include "container/linear_array.hpp"

TEST(test_linear_array, test_linear_array_pair_index) {
  EXPECT_EQ(PairLinearArray::pair_to_index(0,0), 0);

  EXPECT_EQ(PairLinearArray::pair_to_index(0,1), 1);
  EXPECT_EQ(PairLinearArray::PairLinearArray::pair_to_index(1,0), 1);
  EXPECT_EQ(PairLinearArray::pair_to_index(1,1), 2);

  EXPECT_EQ(PairLinearArray::pair_to_index(0,2), 3);
  EXPECT_EQ(PairLinearArray::pair_to_index(2,0), 3);
  EXPECT_EQ(PairLinearArray::pair_to_index(2,1), 4);
  EXPECT_EQ(PairLinearArray::pair_to_index(1,2), 4);
  EXPECT_EQ(PairLinearArray::pair_to_index(2,2), 5);
}

TEST(test_linear_array, test_linear_array_full_conn_index) {
  FullConnLinearArray arr(3);

  EXPECT_EQ(arr.pair_to_index(0,0), 0);
  EXPECT_EQ(arr.pair_to_index(1,0), 1);
  EXPECT_EQ(arr.pair_to_index(2,0), 2);

  EXPECT_EQ(arr.pair_to_index(0,1), 3);
  EXPECT_EQ(arr.pair_to_index(1,1), 4);
  EXPECT_EQ(arr.pair_to_index(2,1), 5);

  EXPECT_EQ(arr.pair_to_index(0,2), 6);
  EXPECT_EQ(arr.pair_to_index(1,2), 7);
  EXPECT_EQ(arr.pair_to_index(2,2), 8);
}
