//
// Created by genshen on 2018-05-21.
//

#ifndef CRYSTAL_MD_EAM_H
#define CRYSTAL_MD_EAM_H

#include "eam_one_way.h"
#include "eam_phi.h"
#include "types.h"

class eam {
  friend class SetflParser; // todo better permission.

public:
  EamPhiList eam_phi; // pair potentials for N types elements.

  OneWayEamList electron_density; //!< 电子云密度
  OneWayEamList embedded;         //!< 嵌入能

  /**
   * create a new eam instance by the elements count from root processor with size {@var n_ele} types elements.
   * @param n_ele atom elements count on root processor, only rank @param root is correct(other processors are zero
   * values).
   * @param root the root processor which has the correct element count @param n_ele.
   * @param rank current processor rank.
   * @param comm mpi communicator.
   * @return eam instance.
   */
  static eam *newInstance(atom_type::_type_atom_types n_ele_root, const int root, const int rank, MPI_Comm comm);

  /**
   * initialize eam object with count/size of atom elements.
   * In the constructor, member @var eam_phi, @var electron_density and @var embedded will be initialized by @param
   * n_ele.
   * @param n_ele the count of elements.
   */
  explicit eam(const atom_type::_type_atom_types n_ele);

  void eamBCast(const int root, const int rank, MPI_Comm comm);

  void interpolateFile();

  double toForce(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to, const double dist2,
                 const double df_from, const double df_to);

  /**
   * compute the contribution to electron charge density from atom j of type {@var _atom_key} at location of one atom i.
   * whose distance is specified by {@var dist2}
   * @param _atom_key atom type of atom j.
   * @param dist2 the square of the distance between atom i and atom j.
   * @return the contribution to electron charge density from atom j.
   */
  double chargeDensity(const atom_type::_type_prop_key _atom_key, const double dist2);

  /**
   * compute derivative of embedded energy of atom of type {@var _atom_type},
   * whose electron charge density contributed by its neighbor atoms is specified by {@var rho}.
   * @param _atom_key atom type
   * @param rho  electron charge density contributed by all its neighbor atoms.
   * @return derivative of embedded energy of this atom.
   */
  double dEmbedEnergy(const atom_type::_type_prop_key _atom_key, const double rho);

  /**
   * compute embedded energy of atom of type {@var _atom_type}
   * @param _atom_key n atom type
   * @param rho electron charge density contributed by all its neighbor atoms.
   * @param max_rho the max value of rho in `embedded` table. Because rho may exceed the table, we add a liner term to correct it.
   * @return embedded energy of this atom.
   */
  double embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho, const double max_rho);

  /**
   * pair potential energy.
   * @return pair potential energy.
   */
  double pairPotential(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to,
                       const double dist2);

  /**
   * @deprecated
   * @param _latticeType
   */
  void setlatticeType(char *_latticeType);

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
  std::vector<size_t> dataTableSizes(const std::vector<atom_type::_type_prop_key> &elements);

  /**
   * get the elements count involved in.
   * @return elements count.
   */
  inline atom_type::_type_atom_types geEles() const { return _n_eles; }

private:
  const atom_type::_type_atom_types _n_eles; // the count of element types, which is initialized as 0.
  // all kinds of atoms using the same cutoff.
  char latticeType[8]; //!< 晶格类型  @deprecated
};

#endif // CRYSTAL_MD_EAM_H
