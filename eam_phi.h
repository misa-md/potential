//
// Created by genshen on 2018-05-20.
//

#ifndef CRYSTAL_MD_EAM_HPI_H
#define CRYSTAL_MD_EAM_HPI_H

#include <vector>
#include "../types/atom_types.h"
#include "interpolation_object.h"


class EamPhi : public InterpolationObject {

};

/**
 *  pair potentials for N elements
 */

class EamPhiList {
public:

    /**
     * set the count of atom types.
     * @param types pair potentials with {@var types} types of elements.
     */
    void setSize(_type_atom_types n_types);

    /**
     * append an eam pair potential of element A to element B to the list.
     * this method must be called after calling {@func setSize()}
     * @param type_from one element
     * @param type_to another element
     * @param nR the count/size of data.
     * @param x0 starting point
     * @param dr delta r, the length of two neighboring data.
     * @param buf data buffer
     */
    void append(atom_type::atom_type type_from, atom_type::atom_type type_to,
                int nR, double x0, double dr, double *buf);

    void append(atom_type::atom_type type_from, atom_type::atom_type type_to, EamPhi &phi);

    /**
     * sync all EamPhi in vector to other processors.
     * @param rank MPI rank id.
     */
    void sync(int rank); // this methos must be called after calling {@func setSize}.

    /**
     * initialize  vector {@var eamPhis}, and sync to other processors.
     * @param n_types
     * @param rank
     */
    void sync(_type_atom_types n_types, int rank);

    void interpolateAll();

    /**
     * look up the table to find the pair potentials of two elements with the distance {@var distance}.
     * @param type_from
     * @param type_to
     * @return
     */
    double getPhiByElementType(atom_type::atom_type type_from, atom_type::atom_type type_to, double distance);

    /**
     * @deprecated
     */
    EamPhi *getPhiByEamPhiByType(atom_type::atom_type type_from, atom_type::atom_type type_to);

private:
    _type_atom_types n_types;
    std::vector<EamPhi> eamPhis;

    /**
     * Compute the index in vector if two element are type_from and type_tp.
     *
     * Example 3 types of elements (table):
     * Fe: 0, Cu: 1, Ni: 2
     * ------------------------
     * type_from | type_to | index
     * ------------------------
     *  Fe(0)  Fe(0)  0
     *  Cu(1)  Fe(0)  1
     *  Cu(1)  Cu(1)  2
     *  Ni(2)  Fe(0)  3
     *  Ni(2)  Cu(1)  4
     *  Ni(2)  Ni(2)  5
     * ------------------------
     * @param type_from element type A
     * @param type_to another element type B.
     * @return index in vector.
     */
    inline unsigned int index(atom_type::atom_type type_from, atom_type::atom_type type_to) {
        if (type_from < type_to) {
            return type_to * (type_to + 1) / 2 + type_from;
        }
        return type_from * (type_from + 1) / 2 + type_to;
    }
};

#endif //CRYSTAL_MD_EAM_HPI_H
