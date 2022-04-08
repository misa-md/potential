//
// Created by genshen on 5/21/18.
//

#include "eam.h"
#include <cmath>
#include <cstring>
#include <mpi.h>

eam *eam::newInstance(atom_type::_type_atom_types n_ele_root, const int root, const int rank, MPI_Comm comm) {
  // broadcast element count/size.
  MPI_Bcast(&n_ele_root, 1, atom_type::MPI_TYPE_TYPES, root, comm);
  return new eam(n_ele_root); // todo delete
}

eam::eam(const atom_type::_type_atom_types n_ele)
    : eam_phi(n_ele), electron_density(n_ele), embedded(n_ele), _n_eles(n_ele) {}

void eam::setlatticeType(char *_latticeType) { strcpy(latticeType, _latticeType); }

void eam::eamBCast(const int root, const int rank, MPI_Comm comm) {
  electron_density.sync(_n_eles, root, rank, comm);
  embedded.sync(_n_eles, root, rank, comm);
  eam_phi.sync(_n_eles, root, rank, comm);
}

void eam::interpolateFile() {
  electron_density.interpolateAll();
  embedded.interpolateAll();
  eam_phi.interpolateAll();
}

double eam::toForce(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to,
                    const double dist2, const double df_from, const double df_to) {
  double fpair;
  double phi, phip, psip, z2, z2p;

  const InterpolationObject *phi_spline = eam_phi.getPhiByEamPhiByType(key_from, key_to);
  const InterpolationObject *electron_spline_from = electron_density.getEamItemByType(key_from);
  const InterpolationObject *electron_spline_to = electron_density.getEamItemByType(key_to);

  const double r = sqrt(dist2);
  const SplineData phi_s = phi_spline->findSpline(r);
  const SplineData ele_from_s = electron_spline_from->findSpline(r);
  const SplineData ele_to_s = electron_spline_to->findSpline(r);

  z2 = ((phi_s.spline[3] * phi_s.p + phi_s.spline[4]) * phi_s.p + phi_s.spline[5]) * phi_s.p + phi_s.spline[6];
  // z2 = phi*r
  z2p = (phi_s.spline[0] * phi_s.p + phi_s.spline[1]) * phi_s.p + phi_s.spline[2];
  // z2p = (phi * r)' = (phi' * r) + phi

  const double rho_p_from =
      (ele_from_s.spline[0] * ele_from_s.p + ele_from_s.spline[1]) * ele_from_s.p + ele_from_s.spline[2];
  const double rho_p_to = (ele_to_s.spline[0] * ele_to_s.p + ele_to_s.spline[1]) * ele_to_s.p + ele_to_s.spline[2];

  const double recip = 1.0 / r;
  phi = z2 * recip;                 // pair potential energy
  phip = z2p * recip - phi * recip; // phip = phi' = (z2p - phi)/r
  psip = df_from * rho_p_to + df_to * rho_p_from + phip;
  fpair = -psip * recip;

  return fpair;
}

double eam::chargeDensity(const atom_type::_type_prop_key _atom_key, const double dist2) {
  const InterpolationObject *electron_spline = electron_density.getEamItemByType(_atom_key);
  const double r = sqrt(dist2);
  const SplineData s = electron_spline->findSpline(r);
  return ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6];
}

double eam::dEmbedEnergy(const atom_type::_type_prop_key _atom_key, const double rho) {
  const InterpolationObject *embed = embedded.getEamItemByType(_atom_key);
  const SplineData s = embed->findSpline(rho);
  return (s.spline[0] * s.p + s.spline[1]) * s.p + s.spline[2];
}

double eam::embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho, const double max_rho) {
  const InterpolationObject *embed = embedded.getEamItemByType(_atom_key);
  const SplineData s = embed->findSpline(rho);
  double phi = ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6];
  if (rho > max_rho) {
    // rho will exceed table, we add a linear term to it.
    const double fp = (s.spline[0] * s.p + s.spline[1]) * s.p + s.spline[2]; // or: dEmbedEnergy(_atom_key, rho);
    phi += fp * (rho - max_rho);
  }
  return phi;
}

double eam::pairPotential(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to,
                          const double dist2) {
  const InterpolationObject *phi_spline = eam_phi.getPhiByEamPhiByType(key_from, key_to);
  const double r = sqrt(dist2);

  const SplineData s = phi_spline->findSpline(r);
  const double phi_r = ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6]; // pair_pot * r
  return phi_r / r;
}

std::vector<size_t> eam::dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) {
  const size_t n_eles = elements.size();
  std::vector<size_t> sizes;
  sizes.reserve((2 * n_eles + (n_eles + 1) * n_eles / 2));
  // for data size of electron charge density
  for (atom_type::_type_prop_key ele : elements) {
    sizes.emplace_back(electron_density.getEamItemByType(ele)->n);
  }
  // for data size of embedded energy
  for (atom_type::_type_prop_key ele : elements) {
    sizes.emplace_back(electron_density.getEamItemByType(ele)->n);
  }
  // for data size of pair potential
  for (size_t i = 0; i < n_eles; i++) {
    for (size_t j = 0; j < i; j++) {
      sizes.emplace_back(eam_phi.getPhiByEamPhiByType(elements[i], elements[j])->n);
    }
  }
  return sizes;
}
