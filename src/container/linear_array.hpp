//
// Created by genshen on 2022/9/24.
//

#ifndef POT_LINEAR_ARRAY_HPP
#define POT_LINEAR_ARRAY_HPP

#include <vector>

#include "interpolation_object.h"
#include "types.h"

/**
 * In linear array, the key can be directly used for indexing interpolation data in data array.
 *
 * \note: The linear array is a new implementation for replacing the old implementation
 * of @class InterpolationLists and it relative class.
 * The difference is that:
 * For InterpolationLists, the interpolations for each elements pair are store as key-value pair list,
 * in which the keys can be any integer or integer pair (e.g. atomic number as key).
 * But in LinearArray, the key must be integer and must start from zero.
 */
class LinearArray {
public:
  explicit LinearArray(const int n_eles) : n_elements(n_eles){};

  void push_back(int nR, double x0, double dr, double *buf) {
    InterpolationObject *inter_obj = new InterpolationObject();
    inter_obj->initInterpolationObject(nR, x0, dr, buf);
    data.emplace_back(inter_obj);

    len++;
  }

  /**
   * For api compatibility for deprecated OneWayEam and EamPhi.
   * The key is useless.
   */
  template <typename KEY> inline void append(const KEY key, int nR, double x0, double dr, double *buf) {
    push_back(nR, x0, dr, buf);
  }
  /**
   * For api compatibility for deprecated OneWayEam and EamPhi.
   * The key1 and key2 is useless.
   */
  template <typename KEY>
  inline void append(const KEY key1, const KEY key2, int nR, double x0, double dr, double *buf) {
    push_back(nR, x0, dr, buf);
  }

  inline InterpolationObject *operator[](const int k) { return data[k]; }

  inline InterpolationObject *index(const int k) { return data[k]; }

  /**
   * For api compatibility for deprecated OneWayEam
   * @param ele_key
   * @return
   */
  inline InterpolationObject *getEamItemByType(atom_type::_type_prop_key ele_key) { return data[ele_key];
  }

  void interpolateAll() {
    for (array_map::type_map_index i = 0; i < len; i++) {
      data[i]->interpolateFile();
    }
  }

  void sync(const int root, const int rank, MPI_Comm comm) {
    // the size of data elements may be different on each process, make them the same.
    MPI_Bcast(&len, 1, MPI_Type_EleSize, root, comm);
    data.resize(len); // make the size the same on all processors.

    // sync values/data
    for (array_map::type_map_index i = 0; i < len; i++) {
      data[i]->bcastInterpolationObject(root, rank, comm);
    }
  }

  /**
   * For api compatibility for deprecated OneWayEam
   * \param size element size in simulation system.
   * \param root MPI bcast root process
   * \param rank current MPI rank
   * \param comm MPI communicator.
   */
  void sync(const array_map::type_map_size size, const int root, const int rank, MPI_Comm comm) {
    sync(root, rank, comm);
  }

public:
  const int n_elements; // elements number of alloy in the simulation system

protected:
  std::vector<InterpolationObject *> data;

private:
  array_map::type_map_index len = 0;
  const MPI_Datatype MPI_Type_EleSize = MPI_SHORT;
};

class PairLinearArray : public LinearArray {
public:
  explicit PairLinearArray(const int n_eles) : LinearArray(n_eles){};

  /**
   * At full unordered mode,
   * map pair to array index and the reference of the \class InterpolationObject specified by array index.
   * e.g.
   * (key1, key2): (0,0) (1,0), (1,1), (2,0), (2,1), (2,2)
   * array index: 0,1,2,3,4,5
   * @param key1 key1 in the pair
   * @param key2 key2 in the pair
   * @return the reference of InterpolationObject specified by the pair.
   */
  inline InterpolationObject *index(const int key1, const int key2) { return data[pair_to_index(key1, key2)]; }

  /**
   * For api compatibility for deprecated EamPhi
   * @param type_from
   * @param type_to
   * @return
   */
  inline InterpolationObject *getPhiByEamPhiByType(atom_type::_type_prop_key type_from,
                                                   atom_type::_type_prop_key type_to) {
    return index(type_from, type_to);
  }

  /**
   * map pair to array index.
   * @param key1 key1 in the pair
   * @param key2 key2 in the pair
   * @return the reference of InterpolationObject specified by the pair.
   */
  static inline int pair_to_index(const int key1, const int key2) {
    int k = key1 * (key1 + 1) / 2 + key2;
    if (key1 < key2) {
      k = key2 * (key2 + 1) / 2 + key1;
    }
    return k;
  }
};

class FullConnLinearArray : public LinearArray {
public:
  explicit FullConnLinearArray(const int n_eles) : LinearArray(n_eles){};

  /**
   * At full connection mode,
   * map pair to array index and the reference of the \class InterpolationObject specified by array index.
   * e.g. when n_eles is 3, we have:
   * (key1, key2): (0,0) (1,0), (2,0), (0,1), (1,1), (2,1), (0,2), (1,2), (2,2)
   * array index: 0,1,2,3,4,5,4,5,6,7,8
   * @param key_x key1 in the pair
   * @param key_y key2 in the pair
   * @return the reference of InterpolationObject specified by the pair.
   */
  inline InterpolationObject *index(const int key_x, const int key_y) { return data[pair_to_index(key_x, key_y)]; }

  /**
   * map pair to array index.
   * @param key_x key1 in the pair
   * @param key_y key2 in the pair
   * @return the reference of InterpolationObject specified by the pair.
   */
  inline int pair_to_index(const int key_x, const int key_y) { return n_elements * key_y + key_x; }
};

#endif // POT_LINEAR_ARRAY_HPP
