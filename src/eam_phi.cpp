//
// Created by genshen on 2018-05-20.
//
#include "eam_phi.h"

EamPhiList::EamPhiList(atom_type::_type_atom_types size)
        : eam_phis(size * (size + 1) / 2) {}

void EamPhiList::append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to,
                        int nR, double x0, double dr, double *buf) {
    array_map::type_map_index i = eam_phis.insert(_type_two_way_key(type_from, type_to), EamPhi{});
    if (i == _type_two_way_map::INDEX_UN_LIMITED) {
        return; // todo catch insert fail?
    }
    eam_phis.elements[i].initInterpolationObject(nR, x0, dr, buf);
}

void
EamPhiList::append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to, EamPhi &phi) {
    eam_phis.insert(_type_two_way_key(type_from, type_to), phi);
}

void EamPhiList::interpolateAll() {
    for (array_map::type_map_index i = 0; i < eam_phis.size(); i++) {
        eam_phis.elements[i].interpolateFile();
    }
}

EamPhi *
EamPhiList::getPhiByEamPhiByType(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to) {
    return eam_phis.get(_type_two_way_key(type_from, type_to)); // todo export interface of get.
}
