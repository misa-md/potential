//
// Created by genshen on 2022/9/25.
//

#include "pot_table_loader.h"

template <typename TYPE_ELE, typename TYPE_EMBED, typename TYPE_PHI>
void EamBaseLoader<TYPE_ELE, TYPE_EMBED, TYPE_PHI>::interpolateFile() {
  electron_density.interpolateAll();
  embedded.interpolateAll();
  eam_phi.interpolateAll();
}

template <typename TYPE_ELE, typename TYPE_EMBED, typename TYPE_PHI>
void EamBaseLoader<TYPE_ELE, TYPE_EMBED, TYPE_PHI>::eamBCast(const int root, const int rank, MPI_Comm comm) {

  electron_density.sync(_n_eles, root, rank, comm);
  embedded.sync(_n_eles, root, rank, comm);
  eam_phi.sync(_n_eles, root, rank, comm);
}

template <typename TYPE_ELE, typename TYPE_EMBED, typename TYPE_PHI>
EamBaseLoader<TYPE_ELE, TYPE_EMBED, TYPE_PHI>::EamBaseLoader(const atom_type::_type_atom_types n_ele)
    : eam_phi(n_ele), electron_density(n_ele), embedded(n_ele), _n_eles(n_ele) {}

template class EamBaseLoader<LinearArray, LinearArray, PairLinearArray>;

template class EamBaseLoader<FullConnLinearArray, LinearArray, PairLinearArray>;

EamAlloyLoader::EamAlloyLoader(const atom_type::_type_atom_types n_ele)
    : EamBaseLoader<LinearArray, LinearArray, PairLinearArray>(n_ele) {}

std::vector<size_t> EamAlloyLoader::dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) {
  const size_t n_eles = elements.size();
  std::vector<size_t> sizes;
  sizes.reserve((2 * n_eles + (n_eles + 1) * n_eles / 2));
  // for data size of electron charge density
  for (atom_type::_type_prop_key ele : elements) {
    sizes.emplace_back(electron_density.getEamItemByType(ele)->n);
  }
  // for data size of embedded energy
  for (atom_type::_type_prop_key ele : elements) {
    sizes.emplace_back(electron_density.getEamItemByType(ele)->n);
  }
  // for data size of pair potential
  for (size_t i = 0; i < n_eles; i++) {
    for (size_t j = 0; j < i; j++) {
      sizes.emplace_back(eam_phi.getPhiByEamPhiByType(elements[i], elements[j])->n);
    }
  }
  return sizes;
}

EamFsLoader::EamFsLoader(const atom_type::_type_atom_types n_ele)
    : EamBaseLoader<FullConnLinearArray, LinearArray, PairLinearArray>(n_ele) {}
std::vector<size_t> EamFsLoader::dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements) {
  // todo:
  return std::vector<size_t>();
}
