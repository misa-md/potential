//
// Created by genshen on 2023/12/21.
//

#include <eam.h>
#include <fstream>
#include <iostream>
#include <string>

#include "eam_c_api.h"
#include "parser/setfl_parser.h"

eam *_pot = nullptr;

int eam_pot_init(char *pot_file_path, const int eam_style, int own_rank, int master_rank, MPI_Comm mpi_comm) {
  const std::string filepath = std::string(pot_file_path);

  // read the potential file
  if (own_rank == master_rank) {
    std::ifstream pot_file;
    pot_file.open(filepath, std::ios::in);
    if (!pot_file.is_open()) {
      std::cerr << "open potential file `" << filepath << "` failed" << std::endl;
      MPI_Abort(mpi_comm, 1);
      return 1;
    }

    // new parser
    SetflParser *parser = new SetflParser(pot_file); // todo delete (vector)
    parser->parseHeader();                           // todo parsing error.

    _pot = eam::newInstance(eam_style, parser->getEles(), master_rank, own_rank, mpi_comm);
    // read potential data
    parser->parseBody(_pot); // todo parsing error.
    pot_file.close();
  } else {
    _pot = eam::newInstance(eam_style, 0, master_rank, own_rank, mpi_comm);
  }
  // BCast Potential
  _pot->eamBCast(master_rank, own_rank, mpi_comm);
  _pot->interpolateFile(); // interpolation.
  return 0;
}

void eam_pot_destroy() { delete _pot; }

double eam_pot_charge_density_alloy(const tp_pot_atom_type _atom_key, const double dist2) {
  return _pot->chargeDensity(_atom_key, dist2);
}

double eam_pot_charge_density_fs(const tp_pot_atom_type _atom_key1, const tp_pot_atom_type _atom_key2,
                                 const double dist2) {
  return _pot->chargeDensity(_atom_key1, _atom_key2, dist2);
}

double eam_pot_d_embed_energy(const tp_pot_atom_type _atom_key, const double rho) {
  return _pot->dEmbedEnergy(_atom_key, rho);
}

double eam_pot_embed_energy_with_maxrho(const tp_pot_atom_type _atom_key, const double rho, const double max_rho) {
  return _pot->embedEnergy(_atom_key, rho, max_rho);
}

double eam_pot_embed_energy(const tp_pot_atom_type _atom_key, const double rho) {
  return _pot->embedEnergy(_atom_key, rho);
}

double eam_pot_pair_potential(const tp_pot_atom_type key_from, const tp_pot_atom_type key_to, const double dist2) {
  return _pot->pairPotential(key_from, key_to, dist2);
}

double eam_pot_to_force(const tp_pot_atom_type key_from, const tp_pot_atom_type key_to, const double dist2,
                        const double df_from, const double df_to) {
  return _pot->toForce(key_from, key_to, dist2, df_from, df_to);
}

tp_pot_atom_types_num eam_pot_get_n_eles() { return _pot->geEles(); }
