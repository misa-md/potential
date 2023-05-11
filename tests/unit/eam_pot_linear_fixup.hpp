//
// Created by genshen on 2023-5-11.
//

#ifndef POTENTIAL_GTEST_EAM_POT_LINEAR_FIXUP_H
#define POTENTIAL_GTEST_EAM_POT_LINEAR_FIXUP_H

#include <eam.h>
#include <gtest/gtest.h>
#include <random>

/**
 * EamPotAlloyLinearTestFixture create a linear potential table and provide interpolation for it.
 * In the linear potential table, it will be very easy to check the expected value for each potential category.
 */
class EamPotAlloyLinearTestFixture : public ::testing::Test {
public:
  eam *_pot = nullptr;
  atom_type::_type_prop_key *prop_key_list = nullptr;

  constexpr static unsigned int ELE_SIZE = 3;
  constexpr static unsigned int DATA_SIZE = 5000;
  //  constexpr static unsigned int RAND_SEED = 27321;
  constexpr static double DELTA = 0.1;

  void SetUp() override {
    const unsigned int root_rank = 0;
    int own_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &own_rank);

    _pot = eam::newInstance(EAM_STYLE_ALLOY, ELE_SIZE, root_rank, own_rank, MPI_COMM_WORLD);
    prop_key_list = new atom_type::_type_prop_key[ELE_SIZE];

    // data buffer
    double data_buff_emb[5000] = {};
    double data_buff_elec[5000] = {};
    for (int k = 0; k < 5000; k++) {
      data_buff_emb[k] = gen_embed(k);
      data_buff_elec[k] = gen_electron_density(k);
    }

    auto eam_alloy_loader = dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader);
    for (int i = 0; i < ELE_SIZE; i++) {
      const int key = i; // key is atom number
      prop_key_list[i] = key;
      x0_electron_density.push_back(key);
      x0_embedded.push_back(-key);
      eam_alloy_loader->embedded.append(key, DATA_SIZE, -key, 0.001, data_buff_emb);
      eam_alloy_loader->electron_density.append(key, DATA_SIZE, key, 0.001, data_buff_elec);
    }

    int i, j;
    for (i = 0; i < ELE_SIZE; i++) {
      double data_buff[5000] = {};
      for (int k = 0; k < 5000; k++) {
        data_buff[k] = gen_phi(i, 0, k);
      }
      for (j = 0; j <= i; j++) {
        const double x0 = i * ELE_SIZE + j;
        x0_eam_phi.push_back(x0);
        eam_alloy_loader->eam_phi.append(prop_key_list[i], prop_key_list[j], DATA_SIZE, x0, 0.001, data_buff);
      }
    }

    _pot->eamBCast(root_rank, own_rank, MPI_COMM_WORLD);
    _pot->interpolateFile(); // interpolation
  }

  static inline double gen_electron_density(int k) { return (k * DELTA * 3.0); }
  static inline double gen_embed(int k) { return (-k * DELTA * 10.0); }
  static inline double gen_phi(int key1, int key2, int k) { return (key1 + k * DELTA * 10.0); }

  inline double max_rho_for_embedded(int key) { return -key + 0.001 * DATA_SIZE; }
  inline double max_x_for_elec_density(int key) { return key + 0.001 * DATA_SIZE; }
  inline double max_x_for_pair_phi(int key1, int key2) { return key1 * ELE_SIZE + key2 + 0.001 * DATA_SIZE; }

  /**
   * the expected value for electron_density at a given distance and key.
   */
  inline double expected_electron_density(const double r, const int key) {
    const double x0 = x0_electron_density[key];
    return ((r - x0) / 0.001) * DELTA * 3.0;
  }
  /**
   * the expected value for embedded at a given electron density and key.
   */
  inline double expected_embed(const double rho, const int key) {
    const double x0 = x0_embedded[key];
    return (-(rho - x0) / 0.001) * DELTA * 10.0; // keep the same as gen_embed
  }
  /**
   * the expected value for pair potential at a given distance and two keys.
   */
  inline double expected_phi(const double r, int key1, int key2) {
    if (key1 < key2) {
      std::swap(key1, key2);
    }
    int index = key1 * (key1 + 1) / 2 + key2;
    const double x0 = x0_eam_phi[index];
    const double e = (key1 + (r - x0) / 0.001) * DELTA * 10.0; // keep the same as expected_phi
    return e / r;
  }

  void TearDown() override {
    delete _pot;
    delete[] prop_key_list;
  }

protected:
  std::vector<double> x0_embedded;         // list of x0 of embedded potential table.
  std::vector<double> x0_electron_density; // list of x0 of electron density potential table.
  std::vector<double> x0_eam_phi;          // list of x0 of eam_phi potential table.
};


#endif // POTENTIAL_GTEST_EAM_POT_LINEAR_FIXUP_H
