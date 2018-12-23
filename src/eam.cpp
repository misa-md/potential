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
                    const double dist2, const double df_sum) {
    int nr, m;
    double p;
    double fpair;
    double dRho, phiTmp, dPhi;
    double recip, phi, phip, psip, z2, z2p;
    double (*spline)[7];

    InterpolationObject *phi_spline = eam_phi.getPhiByEamPhiByType(key_from, key_to);
    InterpolationObject *electron_spline = electron_density.getEamItemByType(key_from); // todo which element type?

    double r = sqrt(dist2);
    nr = phi_spline->n;
    p = r * phi_spline->invDx + 1.0;
    m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);
    spline = phi_spline->spline;
    phiTmp = ((spline[m][3] * p + spline[m][4]) * p + spline[m][5]) * p + spline[m][6];
    dPhi = (spline[m][0] * p + spline[m][1]) * p + spline[m][2];
//    spline = rho_spline->spline // fixme  below.
    spline = electron_spline->spline;
    dRho = (spline[m][0] * p + spline[m][1]) * p + spline[m][2];

    z2 = phiTmp;
    z2p = dPhi;
    recip = 1.0 / r;
    phi = z2 * recip;
    phip = z2p * recip - phi * recip;
    psip = df_sum * dRho + phip;
    fpair = -psip * recip;

    return fpair;
}

double eam::rhoContribution(const atom_type::_type_prop_key _atom_key, const double dist2) {
    InterpolationObject *electron_spline = electron_density.getEamItemByType(_atom_key);

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

double eam::embedEnergyContribution(const atom_type::_type_prop_key _atom_key, const double rho) {
    InterpolationObject *embed = embedded.getEamItemByType(_atom_key);
    int nr = embed->n;
    double p = rho * embed->invDx + 1.0;
    int m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);
    return (embed->spline[m][0] * p + embed->spline[m][1]) * p + embed->spline[m][2];
}
