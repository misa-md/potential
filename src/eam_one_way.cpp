//
// Created by genshen on 2018-05-23.
//

#include "eam_one_way.h"

void OneWayEamList::setSize(_type_atom_types n_types) {
    this->n_types = n_types;
    eamItems.resize(n_types);
}

void OneWayEamList::append(atom_type::atom_type ele_type, int nR, double x0, double dr, double *buf) {
    unsigned int i = index(ele_type);
    eamItems[i].initInterpolationObject(nR, x0, dr, buf);
}

void OneWayEamList::append(atom_type::atom_type ele_type, OneWayEam &eam_item) {
    unsigned int i = index(ele_type);
    eamItems[i] = eam_item;
}

void OneWayEamList::sync(int rank) {
    for (OneWayEam &item: eamItems) {
        item.bcastInterpolationObject(rank);
    }
}

void OneWayEamList::sync(_type_atom_types n_types, int rank) {
    if (this->n_types != n_types && eamItems.size() == 0) {
        setSize(n_types);
    }
    sync(rank);
}

void OneWayEamList::interpolateAll() {
    for (OneWayEam &item: eamItems) {
        item.interpolatefile();
    }
}

OneWayEam *OneWayEamList::getEamItemByType(atom_type::atom_type ele_type) {
    unsigned int i = index(ele_type);
    return &eamItems[i];
}
