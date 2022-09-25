//
// Created by genshen on 2020/2/26.
//

#include <eam.h>
#include <iostream>

class EamFixture : public benchmark::Fixture {
public:
    eam *_pot = nullptr;

    void SetUp(const ::benchmark::State &state) {
        const unsigned int ELE_SIZE = 3;
        const unsigned int DATA_SIZE = 5000;
        const unsigned int rook_rank = 0;
        const unsigned int own_rank = 0;
        // todo support MPI: rank and root id
        _pot = eam::newInstance(EAM_STYLE_ALLOY, ELE_SIZE, rook_rank, own_rank, MPI_COMM_WORLD);
        atom_type::_type_prop_key *prop_key_list = new atom_type::_type_prop_key[ELE_SIZE];

        double buff[5000] = {};
        for (int i = 0; i < ELE_SIZE; i++) {
            int key = i; // key is atom number
            prop_key_list[i] = key;
            dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader)->embedded.append(key, DATA_SIZE, 0.0, 0.001, buff);
            dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader)->electron_density.append(key, DATA_SIZE, 0.0, 0.001, buff);
        }

        int i, j;
        for (i = 0; i < ELE_SIZE; i++) {
            for (j = 0; j <= i; j++) {
              dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader)->eam_phi.append(prop_key_list[i], prop_key_list[j], DATA_SIZE, 0.0, 0.001, buff);
            }
        }

        _pot->eamBCast(rook_rank, own_rank, MPI_COMM_WORLD);
        _pot->interpolateFile(); // interpolation
    }

    void TearDown(const ::benchmark::State &state) {
        delete _pot;
    }
};

BENCHMARK_F(EamFixture, FindPhiSpline)(benchmark::State &st) {
    const atom_type::_type_prop_key key_from = 0;
    const atom_type::_type_prop_key key_to = 1;
    for (auto _ : st) {
      dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader)->eam_phi.getPhiByEamPhiByType(key_from, key_to);
    }
}

BENCHMARK_F(EamFixture, FindEmbedSpline)(benchmark::State &st) {
    const atom_type::_type_prop_key key = 0;
    for (auto _ : st) {
        dynamic_cast<EamAlloyLoader *>(_pot->eam_pot_loader)->embedded.getEamItemByType(key);
    }
}
