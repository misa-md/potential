//
// Created by genshen on 2023/12/21.
//

#ifndef POTENTIAL_EAM_C_API_H
#define POTENTIAL_EAM_C_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <mpi.h>

typedef int tp_pot_atom_type;                   // replacement of atom_type::_type_prop_key
typedef tp_pot_atom_type tp_pot_atom_types_num; // replacement of atom_type::_type_atom_types;

/**
 * It will read a file on main process and broadcast the potential data to other processes.
 * It also perform the file interpolating step while data reading and broadcasting.
 * @param pot_file_path the file path of the potential file.
 * @param eam_style the eam style: 0 (eam/alloy, see EAM_STYLE_ALLOY) or 1 (eam/fs, see EAM_STYLE_FS).
 * @param own_rank the rank in mpi communicator @param mpi_comm.
 * @param master_rank the master ran in MPI communicator.
 * @param mpi_comm the MPI communicator used in file reading and broadcasting.
 * @return 0: ok, otherwise: error.
 */
int eam_pot_init(char *pot_file_path, const int eam_style, int own_rank, int master_rank, MPI_Comm mpi_comm);

/**
 * release the memory of eam potential.
 */
void eam_pot_destroy();

/**
 * C api wrapper for cpp api chargeDensity(_atom_key, dist2) in @file eam.h.
 */
double eam_pot_charge_density_alloy(const tp_pot_atom_type _atom_key, const double dist2);

/**
 * C api wrapper for cpp api chargeDensity(_atom_key1, _atom_key2, dist2) in @file eam.h.
 */
double eam_pot_charge_density_fs(const tp_pot_atom_type _atom_key1, const tp_pot_atom_type _atom_key2, const double dist2);

/**
 * C api wrapper for cpp api dEmbedEnergy(_atom_key, rho) in @file eam.h.
 */
double eam_pot_d_embed_energy(const tp_pot_atom_type _atom_key, const double rho);

/**
 * C api wrapper for cpp api embedEnergy(_atom_key, rho, max_rho) (with parameter @param max_rho) in @file eam.h.
 */
double eam_pot_embed_energy_with_maxrho(const tp_pot_atom_type _atom_key, const double rho, const double max_rho);

/**
 * C api wrapper for cpp api embedEnergy(_atom_key, rho) in @file eam.h.
 */
double eam_pot_embed_energy(const tp_pot_atom_type _atom_key, const double rho);

/**
 * C api wrapper for cpp api pairPotential in @file eam.h.
 */
double eam_pot_pair_potential(const tp_pot_atom_type key_from, const tp_pot_atom_type key_to, const double dist2);

/**
 * C api wrapper for cpp api toForce(_atom_key, key_to, dist2, df_from, df_to) in @file eam.h.
 */
double eam_pot_to_force(const tp_pot_atom_type key_from, const tp_pot_atom_type key_to, const double dist2,
                const double df_from, const double df_to);

/**
 * get the elements count involved in.
 * @return elements count.
 */
tp_pot_atom_types_num eam_pot_get_n_eles();

#ifdef __cplusplus
}
#endif

#endif // POTENTIAL_EAM_C_API_H
