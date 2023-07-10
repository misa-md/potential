//
// Created by genshen on 2023/7/10.
//

#include "eam_pot_linear_fixup.hpp"
#include <gtest/gtest.h>

TEST_F(EamPotFsLinearTestFixture, test_eam_fs_embedded_calc) {
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

TEST_F(EamPotFsLinearTestFixture, test_eam_fs_elec_density_calc) {
  // for key = (0, 0)
  {
    constexpr int key1 = 0;
    constexpr int key2 = 0;
    const double max_x = max_x_for_elec_density(key1, key2);
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 5.0) * (max_x / 5.0)),
                     expected_electron_density(max_x / 5.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 16.0) * (max_x / 16.0)),
                     expected_electron_density(max_x / 16.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 32.0) * (max_x / 32.0)),
                     expected_electron_density(max_x / 32.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 2.0) * (max_x / 2.0)),
                     expected_electron_density(max_x / 2.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (2.0 * max_x / 3.0) * (2.0 * max_x / 3.0)),
                     expected_electron_density(2.0 * max_x / 3.0, key1, key2));
  }

  // for key = (2, 1)
  {
    constexpr int key1 = 2;
    constexpr int key2 = 1;
    const double max_x = max_x_for_elec_density(key1, key2);
    constexpr double base = key1 * ELE_SIZE + key2;
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (base + max_x / 5.0) * (base + max_x / 5.0)),
                     expected_electron_density(base + max_x / 5.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 16.0) * (max_x / 16.0)),
                     expected_electron_density(max_x / 16.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 32.0) * (max_x / 32.0)),
                     expected_electron_density(max_x / 32.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (max_x / 2.0) * (max_x / 2.0)),
                     expected_electron_density(max_x / 2.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->chargeDensity(key2, key1, (2.0 * max_x / 3.0) * (2.0 * max_x / 3.0)),
                     expected_electron_density(2.0 * max_x / 3.0, key1, key2));
  }
}

TEST_F(EamPotFsLinearTestFixture, test_eam_fs_pair_phi_calc) {
  // for key1 = 0, key2 = 0
  {
    constexpr int key1 = 0;
    constexpr int key2 = 0;
    const double max_x = max_x_for_pair_phi(key1, key2);
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 5.0) * (max_x / 5.0)),
                     expected_phi(max_x / 5.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 16.0) * (max_x / 16.0)),
                     expected_phi(max_x / 16.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 32.0) * (max_x / 32.0)),
                     expected_phi(max_x / 32.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 2.0) * (max_x / 2.0)),
                     expected_phi(max_x / 2.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (2.0 * max_x / 3.0) * (2.0 * max_x / 3.0)),
                     expected_phi(2.0 * max_x / 3.0, key1, key2));
  }

  // for key1 = 1, key2 = 2
  {
    constexpr int key1 = 1;
    constexpr int key2 = 2;
    const double max_x = max_x_for_pair_phi(key1, key2);
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 5.0) * (max_x / 5.0)),
                     expected_phi(max_x / 5.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 16.0) * (max_x / 16.0)),
                     expected_phi(max_x / 16.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 32.0) * (max_x / 32.0)),
                     expected_phi(max_x / 32.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (max_x / 2.0) * (max_x / 2.0)),
                     expected_phi(max_x / 2.0, key1, key2));
    EXPECT_DOUBLE_EQ(_pot->pairPotential(key1, key2, (2.0 * max_x / 3.0) * (2.0 * max_x / 3.0)),
                     expected_phi(2.0 * max_x / 3.0, key1, key2));
  }
}
