//
// Created by genshen on 2018-12-22.
//

#ifndef POT_ATOM_TYPE_H
#define POT_ATOM_TYPE_H

#include <mpi.h>
#include <string>
#include <climits>

namespace atom_type {
    typedef unsigned short _type_prop_key;
    typedef unsigned short _type_atomic_no;
    typedef unsigned short _type_atom_types; // the count of different atoms.
    typedef _type_atom_types _type_atom_index;

    const MPI_Datatype MPI_TYPE_TYPES = MPI_SHORT; // mpi type of type _type_atom_types.

    const _type_prop_key PropKeyMax = USHRT_MAX;
    /**
     * definition of lattice properties.
     */
    struct AtomProp {
        _type_prop_key id;
        _type_atomic_no no; // atomic number, e.g. 26
        std::string name; // chemical symbol, e.g. "Fe".
        double weight; // standard atomic weight, e.g. 55.845
        double lat_const; // lattice const
        double cut_off; // cut off
        // todo lattice type
    };

    // pre defined lattice prop.
/*    const AtomProp atom_props_list[] = {
            AtomProp{26, 26, "Fe", 55.845},
            AtomProp{26, 26, "Cu", 63.546},
            AtomProp{26, 26, "Mn", 54.938043},
            AtomProp{26, 26, "Ni", 58.6934},
    };

    const AtomProp &Fe = atom_props_list[0];
    const AtomProp &Cu = atom_props_list[1];
    const AtomProp &Ni = atom_props_list[2];
    const AtomProp &Mn = atom_props_list[3];
*/
}
#endif //POT_ATOM_TYPE_H
