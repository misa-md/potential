//
// Created by genshen on 5/21/18.
//

#include <cmath>
#include <cstring>
#include <mpi.h>

#include "container/interpolation_object.h"
#include "eam.h"

eam *eam::newInstance(const int eam_style, atom_type::_type_atom_types n_ele_root, const int root, const int rank,
                      MPI_Comm comm) {
  // broadcast element count/size.
  MPI_Bcast(&n_ele_root, 1, atom_type::MPI_TYPE_TYPES, root, comm);
  return new eam(n_ele_root, eam_style); // todo delete
}

eam::eam(const atom_type::_type_atom_types n_ele, const int eam_style) : _n_eles(n_ele), eam_style(eam_style) {
  if (eam_style == EAM_STYLE_ALLOY) {
    eam_pot_loader = new EamAlloyLoader(n_ele);
  } else if (eam_style == EAM_STYLE_FS) {
    eam_pot_loader = new EamFsLoader(n_ele);
  } else {
    printf("The eam style `%d` is not implemented\n", eam_style);
  }
}

eam::~eam() {
  if (eam_style == EAM_STYLE_ALLOY) {
    dynamic_cast<EamAlloyLoader *>(eam_pot_loader)->destroy();
    delete dynamic_cast<EamAlloyLoader *>(eam_pot_loader);
  } else if (eam_style == EAM_STYLE_FS) {
    dynamic_cast<EamFsLoader *>(eam_pot_loader)->destroy();
    delete dynamic_cast<EamFsLoader *>(eam_pot_loader);
  } else {
    printf("The eam style `%d` is not implemented\n", eam_style);
  }
}

void eam::setlatticeType(char *_latticeType) { strcpy(latticeType, _latticeType); }

double eam::toForce(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to,
                    const double dist2, const double df_from, const double df_to) {
  double fpair;
  double phi, phip, psip, z2, z2p;

  const InterpolationObject *phi_spline = eam_pot_loader->loadEamPhi(key_from, key_to);
  const InterpolationObject *electron_spline_from = ele_charge_load_wrapper(key_from, key_to);
  const InterpolationObject *electron_spline_to = ele_charge_load_wrapper(key_to, key_from);

  const double r = sqrt(dist2);
  const SplineData phi_s = phi_spline->findSpline(r);
  const SplineData ele_from_s = electron_spline_from->findSpline(r);
  const SplineData ele_to_s = electron_spline_to->findSpline(r);

  z2 = ((phi_s.spline[3] * phi_s.p + phi_s.spline[4]) * phi_s.p + phi_s.spline[5]) * phi_s.p + phi_s.spline[6];
  // z2 = phi*r
  z2p = (phi_s.spline[0] * phi_s.p + phi_s.spline[1]) * phi_s.p + phi_s.spline[2];
  // z2p = (phi * r)' = (phi' * r) + phi
  // => phi' = (z2p - phi)/r

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
  const InterpolationObject *electron_spline = eam_pot_loader->loadElectronDensity(_atom_key);
  const double r = sqrt(dist2);
  const SplineData s = electron_spline->findSpline(r);
  return ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6];
}

double eam::chargeDensity(const atom_type::_type_prop_key _atom_key_me, const atom_type::_type_prop_key _atom_key_nei,
                          const double dist2) {
  const InterpolationObject *electron_spline = eam_pot_loader->loadElectronDensity(_atom_key_me, _atom_key_nei);
  const double r = sqrt(dist2);
  const SplineData s = electron_spline->findSpline(r);
  return ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6];
}

double eam::dEmbedEnergy(const atom_type::_type_prop_key _atom_key, const double rho) {
  const InterpolationObject *embed = eam_pot_loader->loadEmbedded(_atom_key);
  const SplineData s = embed->findSpline(rho);
  return (s.spline[0] * s.p + s.spline[1]) * s.p + s.spline[2];
}

double eam::embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho) {
  const InterpolationObject *embed = eam_pot_loader->loadEmbedded(_atom_key);
  return embedEnergyImp(embed, rho, embed->max_val);
}

double eam::embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho, const double max_rho) {
  const InterpolationObject *embed = eam_pot_loader->loadEmbedded(_atom_key);
  return embedEnergyImp(embed, rho, max_rho);
}

double eam::embedEnergyImp(const InterpolationObject *embed, const double rho, const double max_rho) {
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
  const InterpolationObject *phi_spline = eam_pot_loader->loadEamPhi(key_from, key_to);
  const double r = sqrt(dist2);

  const SplineData s = phi_spline->findSpline(r);
  const double phi_r = ((s.spline[3] * s.p + s.spline[4]) * s.p + s.spline[5]) * s.p + s.spline[6]; // pair_pot * r
  return phi_r / r;
}
