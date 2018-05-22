//
// Created by genshen on 2018-05-20.
//

#ifndef CRYSTAL_MD_EAM_HPI_H
#define CRYSTAL_MD_EAM_HPI_H

#include <vector>
#include "../atom/atom_types.h"
#include "interpolation_object.h"


class EamPhi : public InterpolationObject {

};

/**
 *  pair potentials for N elements
 */

class EamPhiList {
public:
    /**
     * initialize pair potentials with {@var types} types of elements.
     * @param types
     */
    EamPhiList(const int n_types);

    EamPhiList();

    /**
     * append an eam pair potential to list.
     * @param nR
     * @param x0 starting point
     * @param dR dr
     * @param buf data buffer
     */
    void append(int nR, double x0, double dR, double *buf);

    /**
     * look up the table to find the pair potentials of two elements with the distance {@var distance}.
     * @param type_from
     * @param type_to
     * @return
     */
    double getPhiByElementType(atom_type::atom_type type_from, atom_type::atom_type type_to, double distance);

private:
    std::vector<EamPhi> eamPhi;
};

#endif //CRYSTAL_MD_EAM_HPI_H
