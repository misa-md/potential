//
// Created by genshen on 2018-12-22.
//

#include <mpi.h>
#include "atom_type_lists.h"

const atom_type::_type_prop_key AtomPropsList::KeyPropNotFound = atom_type::PropKeyMax;
const atom_type::_type_prop_key AtomPropsList::KeyPropExists = atom_type::PropKeyMax - 1;

atom_type::_type_prop_key AtomPropsList::addAtomProp(const atom_type::AtomProp &lp) {
    CHECK_EXIST_OR_RETURN(lp.id, LatPropExists);
    lat_props.push_back(lp);
    return lp.id;
}

atom_type::_type_prop_key AtomPropsList::addAtomProp(const atom_type::_type_atomic_no no, const std::string name,
                                                     const double weight, const double lat_const,
                                                     const double cut_off) {
    const atom_type::_type_prop_key key = makeId(no);
    CHECK_EXIST_OR_RETURN(key, LatPropExists);
    lat_props.push_back(atom_type::AtomProp{key, no, name, weight, lat_const, cut_off});
    return key;
}

atom_type::AtomProp AtomPropsList::findPropByNo(const atom_type::_type_atomic_no no) {
    CHECK_EXIST_OR_RETURN(no, lp); // lp is the matched data.
    return atom_type::AtomProp{LatPropNotFound, 0, "Unknown", 0.0};
}

atom_type::_type_atom_index AtomPropsList::getIndex(const atom_type::_type_prop_key key) {
    atom_type::_type_atom_index i = 0;
    for (const atom_type::AtomProp &lp :lat_props) {
        if (key == lp.id) {
            return i;
        }
        i++;
    }
    return LatPropExists;
}

void AtomPropsList::sync(const int root, const int rank, MPI_Comm comm, const size_t size) {
    if (rank != root) {
        lat_props.resize(size);
    }
    MPI_Bcast(&lat_props[0], sizeof(atom_type::AtomProp) * size, MPI_BYTE, root, comm);
}
