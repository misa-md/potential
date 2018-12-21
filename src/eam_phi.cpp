//
// Created by genshen on 2018-05-20.
//
#include <logs/logs.h>
#include "eam_phi.h"

void EamPhiList::setSize(_type_atom_types n_types) {
    this->n_types = n_types;
    eamPhis.resize(n_types * (n_types + 1) / 2); // n_types + n_types * (n_types - 1) / 2
}

void EamPhiList::append(atom_type::atom_type type_from, atom_type::atom_type type_to,
                        int nR, double x0, double dr, double *buf) {
    unsigned int i = index(type_from, type_to);
    eamPhis[i].initInterpolationObject(nR, x0, dr, buf);
}

void EamPhiList::append(atom_type::atom_type type_from, atom_type::atom_type type_to, EamPhi &phi) {
    unsigned int i = index(type_from, type_to);
    eamPhis[i] = phi;
}

void EamPhiList::sync(int rank) {
    for (EamPhi &phi:eamPhis) {
        phi.bcastInterpolationObject(rank);
    }
}

void EamPhiList::sync(_type_atom_types n_types, int rank) {
    if (this->n_types != n_types && eamPhis.size() == 0) {
        setSize(n_types);
    }
    sync(rank);
}

void EamPhiList::interpolateAll() {
    for (EamPhi &phi:eamPhis) {
        phi.interpolatefile();
    }
}

EamPhi *EamPhiList::getPhiByEamPhiByType(atom_type::atom_type type_from, atom_type::atom_type type_to) {
    unsigned int i = index(type_from, type_to);
    return &eamPhis[i];
}
