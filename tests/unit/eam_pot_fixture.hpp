//
// Created by genshen on 2023-2-26.
//

#ifndef POTENTIAL_GTEST_EAM_POT_FIXTURE_H
#define POTENTIAL_GTEST_EAM_POT_FIXTURE_H

#include <eam.h>
#include <gtest/gtest.h>
#include <random>

class EamPotAlloyTestFixture : public ::testing::Test {
public:
  eam *_pot = nullptr;
  atom_type::_type_prop_key *prop_key_list = nullptr;

  constexpr static unsigned int ELE_SIZE = 3;
  constexpr static unsigned int DATA_SIZE = 5000;
  constexpr static unsigned int RAND_SEED = 27321;
  constexpr static double DELTA = 0.001;

  void SetUp() override {
    const unsigned int root_rank = 0;
    int own_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &own_rank);

    _pot = eam::newInstance(EAM_STYLE_ALLOY, ELE_SIZE, root_rank, own_rank, MPI_COMM_WORLD);
    prop_key_list = new atom_type::_type_prop_key[ELE_SIZE];

    std::mt19937 rng(RAND_SEED);
    std::uniform_real_distribution<double> dist_f(0.0, 5.0);

    // data buffer
    double data_buff_emb[5000] = {};
    double data_buff_elec[5000] = {};
    for (int k = 0; k < 5000; k++) {
      data_buff_emb[k] = k * dist_f(rng);
      data_buff_elec[k] = k * dist_f(rng);
    }

    auto eam_alloy_loader = dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader);
    for (int i = 0; i < ELE_SIZE; i++) {
      const int key = i; // key is atom number
      prop_key_list[i] = key;
      x0_electron_density.push_back(key);
      x0_embedded.push_back(-key);
      eam_alloy_loader->embedded.append(key, DATA_SIZE, -key, DELTA, data_buff_emb);
      eam_alloy_loader->electron_density.append(key, DATA_SIZE, key, DELTA, data_buff_elec);
    }

    int i, j;
    for (i = 0; i < ELE_SIZE; i++) {
      double data_buff[5000] = {};
      for (int k = 0; k < 5000; k++) {
        data_buff[k] = k * dist_f(rng);
      }
      for (j = 0; j <= i; j++) {
        const double x0 = i * ELE_SIZE + j;
        x0_eam_phi.push_back(x0);
        eam_alloy_loader->eam_phi.append(prop_key_list[i], prop_key_list[j], DATA_SIZE, x0, DELTA, data_buff);
      }
    }

    _pot->eamBCast(root_rank, own_rank, MPI_COMM_WORLD);
    _pot->interpolateFile(); // interpolation
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

class EamPotFsTestFixture : public ::testing::Test {
public:
  eam *_pot = nullptr;
  atom_type::_type_prop_key *prop_key_list = nullptr;

  constexpr static unsigned int ELE_SIZE = 3;
  constexpr static unsigned int DATA_SIZE = 5000;
  constexpr static unsigned int RAND_SEED = 27324;
  constexpr static double DELTA = 0.001;

  void SetUp() override {
    const unsigned int root_rank = 0;
    int own_rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &own_rank);

    _pot = eam::newInstance(EAM_STYLE_FS, ELE_SIZE, root_rank, own_rank, MPI_COMM_WORLD);
    prop_key_list = new atom_type::_type_prop_key[ELE_SIZE];

    std::mt19937 rng(RAND_SEED);
    std::uniform_real_distribution<double> dist_f(0.0, 5.0);

    // data buffer
    double data_buff_emb[5000] = {};
    double data_buff_elec[5000] = {};
    for (int k = 0; k < 5000; k++) {
      data_buff_emb[k] = k * dist_f(rng);
      data_buff_elec[k] = k * dist_f(rng);
    }

    auto eam_fs_loader = dynamic_cast<EamFsLoader *>(_pot->eam_pot_loader);
    for (int i = 0; i < ELE_SIZE; i++) {
      const int key = i; // key is atom number
      prop_key_list[i] = key;
      x0_embedded.push_back(key);
      eam_fs_loader->embedded.append(key, DATA_SIZE, key, DELTA, data_buff_emb);
      // add electron density potential
      for (int j = 0; j < ELE_SIZE; j++) {
        const double x0_ele = -(i * ELE_SIZE + j);
        x0_electron_density.push_back(x0_ele);
        eam_fs_loader->electron_density.append(key, DATA_SIZE, x0_ele, DELTA, data_buff_elec);
      }
    }

    int i, j;
    for (i = 0; i < ELE_SIZE; i++) {
      double data_buff[5000] = {};
      for (int k = 0; k < 5000; k++) {
        data_buff[k] = k * dist_f(rng);
      }
      for (j = 0; j <= i; j++) {
        const double x0 = i * ELE_SIZE + j;
        x0_eam_phi.push_back(x0);
        eam_fs_loader->eam_phi.append(prop_key_list[i], prop_key_list[j], DATA_SIZE, x0, DELTA, data_buff);
      }
    }

    _pot->eamBCast(root_rank, own_rank, MPI_COMM_WORLD);
    _pot->interpolateFile(); // interpolation
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

#endif // POTENTIAL_GTEST_EAM_POT_FIXTURE_H
