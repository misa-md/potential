//
// Created by genshen on 2018-05-20.
//
#include "eam_phi.h"

EamPhiList::EamPhiList(atom_type::_type_atom_types size)
        : eam_phis(size * (size + 1) / 2) {}

//void EamPhiList::setSize(_type_atom_types n_types) {
//    this->n_types = n_types;
//    eamPhis.resize(n_types * (n_types + 1) / 2); // n_types + n_types * (n_types - 1) / 2
//}

void EamPhiList::append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to,
                        int nR, double x0, double dr, double *buf) {
    array_map::type_map_index i = eam_phis.insert(_type_two_way_key(type_from, type_to), EamPhi{});
    if (i == _type_two_way_map::INDEX_UN_LIMITED) {
        return; // todo catch insert fail?
    }
    eam_phis.elements[i].initInterpolationObject(nR, x0, dr, buf);
}

void EamPhiList::append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to, EamPhi &phi) {
    eam_phis.insert(_type_two_way_key(type_from, type_to), phi);
}

void EamPhiList::sync(const int root, const int rank, MPI_Comm comm) {
    for (array_map::type_map_index i = 0; i < eam_phis.size(); i++) {
        eam_phis.elements[i].bcastInterpolationObject(root, rank, comm);
    }
}

// fixme
void EamPhiList::sync(atom_type::_type_atom_types n_types, const int root, const int rank, MPI_Comm comm) {
//    if (this->n_types != n_types && eamPhis.size() == 0) {
//        setSize(n_types);
//    }
    sync(root, rank, comm);
}

void EamPhiList::interpolateAll() {
    for (array_map::type_map_index i = 0; i < eam_phis.size(); i++) {
        eam_phis.elements[i].interpolatefile();
    }
}

EamPhi *EamPhiList::getPhiByEamPhiByType(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to) {
    return eam_phis.get(_type_two_way_key(type_from, type_to)); // todo export interface of get.
}
