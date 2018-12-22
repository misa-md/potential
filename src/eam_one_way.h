//
// Created by genshen on 5/23/18.
//

#ifndef CRYSTAL_MD_EAM_RHO_H
#define CRYSTAL_MD_EAM_RHO_H

#include <vector>
#include "data_structure/array_map.hpp"
#include "interpolation_object.h"
#include "atom_type.h"

class OneWayEam : public InterpolationObject {

};

typedef ArrayMap<atom_type::_type_prop_key, OneWayEam> _type_one_way_map;

/**
 * electron charge density and embedded energy items for N elements.
 */
class OneWayEamList {
public:
    explicit OneWayEamList(atom_type::_type_atom_types size);
//    void setSize(atom_type::_type_atom_types n_types);

    /**
     * insert an empty @class OneWayEam, and initialize it with parameters:@param nR,x0,dr,buf
     * @param ele_key key
     * @param nR
     * @param x0
     * @param dr
     * @param buf
     */
    void append(atom_type::_type_prop_key ele_key, int nR, double x0, double dr, double *buf);

    /**
     * insert an @class OneWayEam object with initialized data.
     * @param ele_key key
     * @param eam_item
     */
    void append(atom_type::_type_prop_key ele_key, OneWayEam &eam_item);

    /**
     * sync all data to all other processors.
     * @param root the root processor.
     * @param rank MPI rank id of current processor.
     * @param comm communicator.
     */
    void sync(const int root, const int rank, MPI_Comm comm);

    /**
     * @deprecated
     * initialize vector {@var eamPhis}, and sync to other processors.
     * @param n_types the size of elements.
     * @param root the root processor.
     * @param rank MPI rank id of current processor.
     * @param comm communicator.
     */
    void sync(atom_type::_type_atom_types n_types, const int root, const int rank, MPI_Comm comm);

    void interpolateAll();

    /**
     * @deprecated
     */
    OneWayEam *getEamItemByType(atom_type::_type_prop_key ele_key);

private:
//    const atom_type::_type_atom_types capacity; // the max capacity of vector
    //atom_type::_type_atom_types n_types; // the real size in vector.
    // std::vector<OneWayEam> eamItems;
    _type_one_way_map eam_items;
};


#endif //CRYSTAL_MD_EAM_RHO_H
