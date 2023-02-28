//
// Created by genshen on 2022/9/25.
//

#ifndef POT_POT_TABLE_LOADER_H
#define POT_POT_TABLE_LOADER_H

#include "container/linear_array.hpp"

template <typename TYPE_ELE, typename TYPE_EMBED, typename TYPE_PHI> class EamBaseLoader {
public:
  explicit EamBaseLoader(const atom_type::_type_atom_types n_ele);

  /**
   * given a list of elements, it returns size of the origin data for each element.
   * For example, the length of input elements is N,
   * then the return value will contain (N+N+N*(N+1)/2) values,
   * and each of them represent the size of corresponding origin potential data table.
   * They are listed as following:
   * N data size for electron charge density; N data size for embedded energy and N*(N+1)/2 data size for pair potential
   * energy.
   * @param elements the elements list
   * @return size of the origin data tables for each element.
   */
  virtual std::vector<size_t> dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) = 0;

  /**
   * broadcast the potential table read from file in root MPI process to other processes.
   * @param root
   * @param rank
   * @param comm
   */
  void eamBCast(const int root, const int rank, MPI_Comm comm);

  /**
   * perform spline interpolation on the potential table.
   */
  void interpolateFile();

  /**
   * remove all potentials data (e.g. spline interpolation and the origin potential table)
   */
  void destroy();

public:
  TYPE_PHI eam_phi;          // pair potentials for N types elements.
  TYPE_ELE electron_density; // electron charge density
  TYPE_EMBED embedded;       // embedded energy

protected:
  const atom_type::_type_atom_types _n_eles; // the count of element types, which is initialized as 0.
};

class EamPotTableLoaderApi {
public:
  /**
   * Load potential table of electron density for eam/alloy type.
   * @param _atom_key the type of neighbor atom.
   * @return the potential table rho{atom_key}.
   */
  virtual InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key _atom_key) = 0;

  /**
   * Load potential table of electron density for eam/fs type.
   * load rho{key_from, key_to}.
   * @param key_from the type of the current or center atom.
   * @param key_to the type of the neighbor atom.
   * @return the potential table rho{key_from, key_to}.
   */
  virtual InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key key_from,
                                                   const atom_type::_type_prop_key key_to) = 0;

  /**
   * Load embedded potential table for both eam/alloy and eam/fs type.
   * @param _atom_key atom type.
   * @return the embedded potential table.
   */
  virtual InterpolationObject *loadEmbedded(const atom_type::_type_prop_key _atom_key) = 0;

  /**
   * Load potential table of pair potential for both eam/alloy and eam/fs type.
   * Load the pair potential table phi_{key_from, key_to} or phi_{key_to, key_from}.
   * Note: phi_{key_from, key_to} is the same as phi_{key_to, key_from}, thus, user can
   * call loadEamPhi(key_from, key_to) or loadEamPhi(key_to, key_from).
   *
   * @param key_from center atom type, or neighbor atom type.
   * @param key_to neighbor atom type, or center atom type.
   * @return the pair potential table.
   */
  virtual InterpolationObject *loadEamPhi(const atom_type::_type_prop_key key_from,
                                          const atom_type::_type_prop_key key_to) = 0;
};

class EamAlloyLoader : public EamBaseLoader<LinearArray, LinearArray, PairLinearArray>, public EamPotTableLoaderApi {
public:
  explicit EamAlloyLoader(const atom_type::_type_atom_types n_ele);

  inline InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key _atom_key) override {
    return electron_density.index(_atom_key);
  }

  inline InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key key_from,
                                                  const atom_type::_type_prop_key key_to) override {
    printf("unimplemented electron density.\n");
    return nullptr;
  }

  inline InterpolationObject *loadEmbedded(const atom_type::_type_prop_key _atom_key) override {
    return embedded.index(_atom_key);
  }

  inline InterpolationObject *loadEamPhi(const atom_type::_type_prop_key key_from,
                                         const atom_type::_type_prop_key key_to) override {
    return eam_phi.index(key_from, key_to);
  }

  std::vector<size_t> dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) override;
};

class EamFsLoader : public EamBaseLoader<FullConnLinearArray, LinearArray, PairLinearArray>,
                    public EamPotTableLoaderApi {

public:
  explicit EamFsLoader(const atom_type::_type_atom_types n_ele);

  inline InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key _atom_key) override {
    printf("unimplemented electron density.\n");
    return nullptr;
  }

  inline InterpolationObject *loadElectronDensity(const atom_type::_type_prop_key key_from,
                                                  const atom_type::_type_prop_key key_to) override {
    return electron_density.index(key_from, key_to);
  }

  inline InterpolationObject *loadEmbedded(const atom_type::_type_prop_key _atom_key) override {
    return embedded.index(_atom_key);
  }

  inline InterpolationObject *loadEamPhi(const atom_type::_type_prop_key key_from,
                                         const atom_type::_type_prop_key key_to) override {
    return eam_phi.index(key_from, key_to);
  }

  std::vector<size_t> dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) override;
};

#endif // POT_POT_TABLE_LOADER_H
