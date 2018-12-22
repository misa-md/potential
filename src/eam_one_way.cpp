//
// Created by genshen on 2018-05-23.
//

#include "eam_one_way.h"

OneWayEamList::OneWayEamList(atom_type::_type_atom_types size)
        : eam_items(size) {}

void OneWayEamList::append(atom_type::_type_prop_key ele_key, int nR, double x0, double dr, double *buf) {
    array_map::type_map_index i = eam_items.insert(ele_key, OneWayEam{});
    if (i == _type_one_way_map::INDEX_UN_LIMITED) {
        return; // todo catch insert fail?
    }
    eam_items.elements[i].initInterpolationObject(nR, x0, dr, buf);
}

void OneWayEamList::append(atom_type::_type_prop_key ele_key, OneWayEam &eam_item) {
    eam_items.insert(ele_key, eam_item); // todo catch insert fail?
}

void OneWayEamList::sync(const int root, const int rank, MPI_Comm comm) {
    for (array_map::type_map_index i = 0; i < eam_items.size(); i++) {
        eam_items.elements[i].bcastInterpolationObject(root, rank, comm);
    }
}

void OneWayEamList::sync(atom_type::_type_atom_types n_types,
                         const int root, const int rank, MPI_Comm comm) {
    sync(root, rank, comm);
}

void OneWayEamList::interpolateAll() {
    for (array_map::type_map_index i = 0; i < eam_items.size(); i++) {
        eam_items.elements[i].interpolatefile();
    }
}

OneWayEam *OneWayEamList::getEamItemByType(atom_type::_type_prop_key ele_key) {
    return eam_items.get(ele_key); // todo export interface of get.
}
