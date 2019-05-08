//
// Created by genshen on 5/21/18.
//

#include <mpi.h>
#include <cstring>
#include <cmath>
#include "eam.h"
#include "utils.h"

eam *eam::newInstance(atom_type::_type_atom_types n_ele_root,
                      const int root, const int rank, MPI_Comm comm) {
    // broadcast element count/size.
    MPI_Bcast(&n_ele_root, 1, atom_type::MPI_TYPE_TYPES, root, comm);
    return new eam(n_ele_root); // todo delete
}

eam::eam(const atom_type::_type_atom_types n_ele)
        : eam_phi(n_ele), electron_density(n_ele), embedded(n_ele), _n_eles(n_ele) {}

void eam::setlatticeType(char *_latticeType) {
    strcpy(latticeType, _latticeType);
}

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
    int nr, m;
    double p;
    double fpair;
    double recip, phi, phip, psip, z2, z2p;
    double (*spline)[7];

    const InterpolationObject *phi_spline = eam_phi.getPhiByEamPhiByType(key_from, key_to);
    const InterpolationObject *electron_spline_from = electron_density.getEamItemByType(key_from);
    const InterpolationObject *electron_spline_to = electron_density.getEamItemByType(key_to);

    double r = sqrt(dist2);
    nr = phi_spline->n;
    p = r * phi_spline->invDx + 1.0;
    m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);

    spline = phi_spline->spline;
    z2 = ((spline[m][3] * p + spline[m][4]) * p + spline[m][5]) * p + spline[m][6]; // z2 = phi*r
    z2p = (spline[m][0] * p + spline[m][1]) * p + spline[m][2]; // z2p = (phi * r)' = (phi' * r) + phi

    // fixme the index m of electron spline should recompute ?
    double *spline_m = electron_spline_from->spline[m];
    const double rho_p_from = (spline_m[0] * p + spline_m[1]) * p + spline_m[2];
    spline_m = electron_spline_to->spline[m];
    const double rho_p_to = (spline_m[0] * p + spline_m[1]) * p + spline_m[2];

    recip = 1.0 / r;
    phi = z2 * recip; // pair potential energy
    phip = z2p * recip - phi * recip; // phip = phi' = (z2p - phi)/r
    psip = df_from * rho_p_to + df_to * rho_p_from + phip;
    fpair = -psip * recip;

    return fpair;
}

double eam::chargeDensity(const atom_type::_type_prop_key _atom_key, const double dist2) {
    const InterpolationObject *electron_spline = electron_density.getEamItemByType(_atom_key);

    double r = sqrt(dist2);
    int nr = electron_spline->n;
    double p = r * electron_spline->invDx + 1.0;
    int m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);

    return ((electron_spline->spline[m][3] * p + electron_spline->spline[m][4]) * p
            + electron_spline->spline[m][5]) * p + electron_spline->spline[m][6];
}

double eam::dEmbedEnergy(const atom_type::_type_prop_key _atom_key, const double rho) {
    const InterpolationObject *embed = embedded.getEamItemByType(_atom_key);
    int nr = embed->n;
    double p = rho * embed->invDx + 1.0;
    int m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);
    // note:
    // embedding energy(phi) can be computed by:
    // phi = ((embed->spline[m][3]*p + embed->spline[m][4])*p + embed->spline[m][5])*p + embed->spline[m][6];
    return (embed->spline[m][0] * p + embed->spline[m][1]) * p + embed->spline[m][2];
}
