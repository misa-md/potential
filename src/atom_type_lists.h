//
// Created by genshen on 2018-12-22.
//

#ifndef POT_ATOM_TYPE_LISTS_H
#define POT_ATOM_TYPE_LISTS_H


#include "eam_phi.h"
#include "atom_type.h"

#define CHECK_EXIST_OR_RETURN(ID, RT) \
for (const atom_type::AtomProp &lp :lat_props) { \
    if (ID == lp.id) { \
    return RT; \
    } \
}

class AtomPropsList {
public:

    static const atom_type::_type_atomic_no LatPropNotFound = 0;
    static const atom_type::_type_prop_key LatPropExists = 0;

    inline static atom_type::_type_prop_key calcPropId(atom_type::_type_atomic_no no) {
        return no;
    }

    /**
     * add an atom property to lat prop list if the same lat is not in the list.
     * @param lp lattice property
     * @return the key of inserted atom props. If the key have already exists, @var LatPropExists will be returned
     */
    atom_type::_type_prop_key addAtomProp(const atom_type::AtomProp &lp);

    /**
     * add an atom property specified by following parameters to lat prop list
     * if the same lat is not exists in the list.
     * The id will be created automatically in this method.
     * @return the key of inserted atom props. If the key have already exists, @var LatPropExists will be returned.
     */
    atom_type::_type_prop_key addAtomProp(const atom_type::_type_atomic_no no, const std::string name,
                                          const double weight, const double lat_const, const double cut_off);

    /**
     * return lattice property specified by atomic number.
     * If not found, an empty/default lattice property(@var LatPropNotFound) will be returned.
     * @param no atomic number
     * @return lattice property
     */
    atom_type::AtomProp findPropByNo(const atom_type::_type_atomic_no no);

    /**
     * @deprecated
     * return the index of matched key in vector @var lat_props
     * @param key prop key.
     * @return the index. If it is not found, 0 will be returned.
     */
    atom_type::_type_atom_index getIndex(const atom_type::_type_prop_key key);

    /**
     * get the size of different lattice props.
     * @return the size
     */
    inline size_t size() {
        return lat_props.size();
    }

    /**
     * broadcast list to other processor.
     * @param master master processor.
     * @param rank current rank id.
     * @param comm MPI Communicators
     * @param size the count of vector.
     */
    void sync(const int root, const int rank, MPI_Comm comm, const size_t size);

private:
    std::vector<atom_type::AtomProp> lat_props;

    /**
     * generate id by atomic number.
     * @param no atomic number
     * @return
     */
    inline atom_type::_type_prop_key makeId(atom_type::_type_atomic_no no) {
        return no; // todo we dont consider the same atomic no, but for different id.
    }
};


#endif //POT_ATOM_TYPE_LISTS_H
