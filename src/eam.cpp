//
// Created by genshen on 5/21/18.
//

#include <utils/mpi_utils.h>
#include <cstring>
#include <cmath>
#include "eam.h"

eam::eam() : _nElems(0), has_initialized(false) {};

eam::~eam() {
//    delete[] embedded;
//    delete[] phi;
//    delete[] electron_density;
    delete[] mass; // fixme origin commented.
};

void eam::setatomicNo(double nAtomic) {
    atomicNo = nAtomic;
}

void eam::setlat(double latticeconst) {
    lat = latticeconst;
}

void eam::setmass(int i, double _mass) {
    mass[i] = _mass;
}

void eam::setlatticeType(char *_latticeType) {
    strcpy(latticeType, _latticeType);
}

void eam::setname(char *_name) {
    strcpy(name, _name);
}

void eam::setcutoff(double _cutoff) {
    cutoff = _cutoff;
}

void eam::initElementN(_type_atom_types n_ele) {
    if (n_ele > 0 && !has_initialized) {
        _nElems = n_ele;
        eam_phi.setSize(n_ele);
        embedded.setSize(n_ele);
        electron_density.setSize(n_ele);
        mass = new double[n_ele]; // todo delete?
        has_initialized = true;
    }
}

void eam::eamBCast(int rank) {
    MPI_Bcast(&_nElems, 1, MPI_INT, MASTER_PROCESSOR, MPI_COMM_WORLD);
    if (rank != MASTER_PROCESSOR) {
        this->initElementN(_nElems); // initialize array for storing eam data.
    }
    MPI_Bcast(mass, _nElems, MPI_DOUBLE, MASTER_PROCESSOR, MPI_COMM_WORLD);

    electron_density.sync(_nElems, rank);
    embedded.sync(_nElems, rank);
    eam_phi.sync(_nElems, rank);
}

void eam::interpolateFile() {
    electron_density.interpolateAll();
    embedded.interpolateAll();
    eam_phi.interpolateAll();
}

double eam::toForce(atom_type::atom_type type_from, atom_type::atom_type type_to,
                    double dist2, double df_sum) {
    int nr, m;
    double p;
    double fpair;
    double dRho, phiTmp, dPhi;
    double recip, phi, phip, psip, z2, z2p;
    double (*spline)[7];

    InterpolationObject *phi_spline = eam_phi.getPhiByEamPhiByType(type_from, type_to);
    InterpolationObject *electron_spline = electron_density.getEamItemByType(type_from); // todo which element type?

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

double eam::rhoContribution(atom_type::atom_type _atom_type, double dist2) {
    InterpolationObject *electron_spline = electron_density.getEamItemByType(_atom_type);

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

double eam::embedEnergyContribution(atom_type::atom_type _atom_type, double rho) {
    InterpolationObject *embed = embedded.getEamItemByType(_atom_type);
    int nr = embed->n;
    double p = rho * embed->invDx + 1.0;
    int m = static_cast<int> (p);
    m = std::max(1, std::min(m, (nr - 1)));
    p -= m;
    p = std::min(p, 1.0);
    return (embed->spline[m][0] * p + embed->spline[m][1]) * p + embed->spline[m][2];
}
