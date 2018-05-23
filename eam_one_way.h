//
// Created by genshen on 5/23/18.
//

#ifndef CRYSTAL_MD_EAM_RHO_H
#define CRYSTAL_MD_EAM_RHO_H

#include <vector>
#include "interpolation_object.h"
#include "../pre_define.h"
#include "../atom/atom_types.h"

class OneWayEam : public InterpolationObject {

};

/**
 * electron charge density and embedded energy items for N elements.
 */
class OneWayEamList {
public:
    void setSize(_type_atom_types n_types);

    void append(atom_type::atom_type ele_type, int nR, double x0, double dr, double *buf);

    void append(atom_type::atom_type ele_type, OneWayEam &eam_item);

    void sync(int rank); // this methos must be called after calling {@func setSize}.

    /**
     * initialize  vector {@var eamPhis}, and sync to other processors.
     * @param n_types
     * @param rank
     */
    void sync(_type_atom_types n_types, int rank);

    void interpolateAll();

    /**
     * @deprecated
     */
    OneWayEam *getEamItemByType(atom_type::atom_type ele_type);

private:
    _type_atom_types n_types;
    std::vector<OneWayEam> eamItems;

    inline unsigned int index(atom_type::atom_type ele_type) {
        return ele_type;
    }
};


#endif //CRYSTAL_MD_EAM_RHO_H
