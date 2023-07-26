//
// Created by genshen on 2018-05-20.
//

#ifndef CRYSTAL_MD_EAM_HPI_H
#define CRYSTAL_MD_EAM_HPI_H

#include "container/interpolation_lists.hpp"
#include "container/interpolation_object.h"
#include "data_structure/array_map.hpp"
#include "types.h"
#include <vector>

class EamPhi : public InterpolationObject {};

// type of combination of two _type_prop_key.
struct KeyDb {
public:
  KeyDb(atom_type::_type_prop_key from, atom_type::_type_prop_key to) : from(from), to(to) {
    // make sure from >= to
    if (from < to) {
      this->from = to;
      this->to = from;
    }
  }

  //    void *operator new[](size_t size) {
  //        void *p = ::new KeyDb[size];
  //        return malloc(size);
  //    }

  atom_type::_type_prop_key from, to;

  inline bool operator==(const KeyDb &r) const { return (from == r.from && to == r.to); }

  // private:
  KeyDb() : from(0), to(0) {} // only used for new array.
};

typedef KeyDb _type_two_way_key;
typedef InterpolationLists<_type_two_way_key, EamPhi> _type_two_way_map;

/**
 * @deprecated
 * InterpolationLists and EamPhiList is deprecated.
 * We now use @class EamPotTableLoaderApi instead.
 *
 * pair potentials for N elements
 */
class EamPhiList {
public:
  /**
   *
   * @param size the count of elements.
   */
  explicit EamPhiList(atom_type::_type_atom_types size);

  /**
   * set the count of atom types.
   * @param types pair potentials with {@var types} types of elements.
   */
  //    void setSize(atom_type::_type_atom_types n_types);

  /**
   * append an eam pair potential of element A to element B to the list.
   * @param type_from one element
   * @param type_to another element
   * @param nR the count/size of data.
   * @param x0 starting point
   * @param dr delta r, the length of two neighboring data.
   * @param buf data buffer
   */
  void append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to, int nR, double x0, double dr,
              double *buf);

  void append(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to, EamPhi &phi);

  void interpolateAll();

  /**
   * look up the table to find the pair potentials of two elements with the distance {@var distance}.
   * @param type_from
   * @param type_to
   * @return
   */
  double getPhiByElementType(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to, double distance);

  EamPhi *getPhiByEamPhiByType(atom_type::_type_prop_key type_from, atom_type::_type_prop_key type_to);

  inline void sync(const atom_type::_type_atom_types eles, const int root, const int rank, MPI_Comm comm) {
    eam_phis.sync(eles * (eles + 1) / 2, root, rank, comm);
  }

private:
  //    atom_type::_type_atom_types n_types;
  //    std::vector<EamPhi> eamPhis;
  _type_two_way_map eam_phis;
};

#endif // CRYSTAL_MD_EAM_HPI_H
