//
// Created by genshen on 2023-5-11.
//

#include "eam_pot_linear_fixup.hpp"
#include <gtest/gtest.h>

TEST_F(EamPotAlloyLinearTestFixture, test_eam_alloy_embedded_calc) {
  // for key = 0
  {
    constexpr int key = 0;
    const double max_rho = max_rho_for_embedded(key);
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 5.0, max_rho), expected_embed(max_rho / 5.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 16.0, max_rho), expected_embed(max_rho / 16.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 32.0, max_rho), expected_embed(max_rho / 32.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 2.0, max_rho), expected_embed(max_rho / 2.0, key));
    EXPECT_DOUBLE_EQ(_pot->embedEnergy(key, 2.0 * max_rho / 3.0, max_rho), expected_embed(2.0 * max_rho / 3.0, key));
  }

  // for key = 2
  {
    constexpr int key = 2;
    const double max_rho = max_rho_for_embedded(key);
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 5.0, max_rho), expected_embed(max_rho / 5.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 16.0, max_rho), expected_embed(max_rho / 16.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 32.0, max_rho), expected_embed(max_rho / 32.0, key));
    EXPECT_EQ(_pot->embedEnergy(key, max_rho / 2.0, max_rho), expected_embed(max_rho / 2.0, key));
    EXPECT_DOUBLE_EQ(_pot->embedEnergy(key, 2.0 * max_rho / 3.0, max_rho), expected_embed(2.0 * max_rho / 3.0, key));
  }
}
