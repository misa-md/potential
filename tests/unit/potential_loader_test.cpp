//
// Created by genshen on 2023-2-26.
//

#include <eam.h>
#include <gtest/gtest.h>

#include "eam_pot_fixture.hpp"

// test eam/alloy potential loader.
TEST_F(EamPotAlloyTestFixture, test_alloy_embedded_loader_test) {
  for (int i = 0; i < ELE_SIZE; i++) {
    InterpolationObject *emb = _pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(emb, nullptr);
    EXPECT_EQ(emb->x0, x0_embedded[i]);
  }
}

TEST_F(EamPotAlloyTestFixture, test_alloy_electron_density_loader_test) {
  for (int i = 0; i < ELE_SIZE; i++) {
    InterpolationObject *electron_density = _pot->eam_pot_loader->loadElectronDensity(i);
    EXPECT_NE(electron_density, nullptr);
    EXPECT_EQ(electron_density->x0, x0_electron_density[i]);
  }
}

void test_common_phi_loader(const int ELE_SIZE, EamPotTableLoaderApi *eam_pot_loader,
                            const std::vector<double> &x0_eam_phi) {
  int x0_index = 0;
  for (int i = 0; i < ELE_SIZE; i++) {
    for (int j = 0; j <= i; j++) {
      // use pair (i,j)
      InterpolationObject *phi = eam_pot_loader->loadEamPhi(i, j);
      EXPECT_NE(phi, nullptr);
      EXPECT_EQ(phi->x0, x0_eam_phi[x0_index]);
      // use pair (j,i) is also ok. We expect phi_{i,j} = phi_{j,i}.
      InterpolationObject *phi2 = eam_pot_loader->loadEamPhi(j, i);
      EXPECT_NE(phi2, nullptr);
      EXPECT_EQ(phi2->x0, x0_eam_phi[x0_index]);

      x0_index++;
    }
  }
}

TEST_F(EamPotAlloyTestFixture, test_alloy_phi_loader_test) {
  test_common_phi_loader(ELE_SIZE, _pot->eam_pot_loader, x0_eam_phi);
}

// test eam/fs potential loader.
TEST_F(EamPotFsTestFixture, test_fs_embedded_loader_test) {
  for (int i = 0; i < ELE_SIZE; i++) {
    InterpolationObject *emb = _pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(emb, nullptr);
    EXPECT_EQ(emb->x0, x0_embedded[i]);
  }
}

TEST_F(EamPotFsTestFixture, test_fs_electron_density_loader_test) {
  int x0_index = 0;
  for (int i = 0; i < ELE_SIZE; i++) {
    for (int j = 0; j < ELE_SIZE; j++) {
      InterpolationObject *electron_density = _pot->eam_pot_loader->loadElectronDensity(j, i);
      EXPECT_NE(electron_density, nullptr);
      EXPECT_EQ(electron_density->x0, x0_electron_density[x0_index]);
      x0_index++;
    }
  }
}

TEST_F(EamPotFsTestFixture, test_fs_phi_loader_test) {
  test_common_phi_loader(ELE_SIZE, _pot->eam_pot_loader, x0_eam_phi);
}
