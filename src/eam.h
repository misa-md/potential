//
// Created by genshen on 2018-05-21.
//

#ifndef CRYSTAL_MD_EAM_H
#define CRYSTAL_MD_EAM_H

#include "pot_table_loader.h"
#include "types.h"

constexpr int EAM_STYLE_ALLOY = 0;
constexpr int EAM_STYLE_FS = 1;

class eam {

public:
  /**
   * create a new eam instance by the elements count from root processor with size {@var n_ele} types elements.
   * @param eam_style eam potential style, can be `EAM_STYLE_ALLOY` or `EAM_STYLE_FS`.
   * @param n_ele atom elements count on root processor, only rank @param root is correct(other processors are zero
   * values).
   * @param root the root processor which has the correct element count @param n_ele.
   * @param rank current processor rank.
   * @param comm mpi communicator.
   * @return eam instance.
   */
  static eam *newInstance(const int eam_style, atom_type::_type_atom_types n_ele_root, const int root, const int rank,
                          MPI_Comm comm);

  /**
   * initialize eam object with count/size of atom elements.
   * In the constructor, member @var eam_phi, @var electron_density and @var embedded will be initialized by @param
   * n_ele.
   * @param n_ele the count of elements.
   */
  explicit eam(const atom_type::_type_atom_types n_ele, const int eam_style);

  /**
   * release memory, including spline data and the origin potential data.
   */
  ~eam();

  inline void eamBCast(const int root, const int rank, MPI_Comm comm) {
    if (eam_style == EAM_STYLE_ALLOY) {
      dynamic_cast<EamAlloyLoader *>(eam_pot_loader)->eamBCast(root, rank, comm);
    } else if (eam_style == EAM_STYLE_FS) {
      dynamic_cast<EamFsLoader *>(eam_pot_loader)->eamBCast(root, rank, comm);
    } else {
      printf("The eam style `%d` is not implemented\n", eam_style);
    }
  }

  inline void interpolateFile() {
    if (eam_style == EAM_STYLE_ALLOY) {
      dynamic_cast<EamAlloyLoader *>(eam_pot_loader)->interpolateFile();
    } else if (eam_style == EAM_STYLE_FS) {
      dynamic_cast<EamFsLoader *>(eam_pot_loader)->interpolateFile();
    } else {
      printf("The eam style `%d` is not implemented\n", eam_style);
    }
  }

  /**
   * Calculate force of two atoms.
   * @param key_from type of the center atom
   * @param key_to type of the neighbor atom
   * @param dist2 distance^2 of the two atoms.
   * @param df_from df of the center atom
   * @param df_to df of the neighbor atom
   * @return the force of the two atoms.
   */
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
   * compute the contribution to electron charge density from atom j of type {@var _atom_key2}
   * at location of one atom i of type {@var _atom_key1}.
   * It is used in eam/fs potential style.
   * @param _atom_key1 atom type of atom i.
   * @param _atom_key2 atom type of atom j.
   * @param dist2 square of distance of the two atoms.
   * @return the contribution to electron charge density
   */
  double chargeDensity(const atom_type::_type_prop_key _atom_key1, const atom_type::_type_prop_key _atom_key2,
                       const double dist2);

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
   * @param max_rho the max value of rho in `embedded` table. Because rho may exceed the table, we add a liner term to
   * correct it.
   * @return embedded energy of this atom.
   */
  double embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho, const double max_rho);

  double embedEnergy(const atom_type::_type_prop_key _atom_key, const double rho);

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
   * get the elements count involved in.
   * @return elements count.
   */
  inline atom_type::_type_atom_types geEles() const { return _n_eles; }

private:
  inline InterpolationObject *ele_charge_load_wrapper(const atom_type::_type_prop_key _atom_key_me,
                                                      const atom_type::_type_prop_key _atom_key_nei) {
    if (eam_style == EAM_STYLE_ALLOY) {
      return eam_pot_loader->loadElectronDensity(_atom_key_me);
    } else if (eam_style == EAM_STYLE_FS) {
      return eam_pot_loader->loadElectronDensity(_atom_key_me, _atom_key_nei);
    } else {
      printf("error: unspecified eam style.\n");
      return nullptr;
    }
  }

  static double embedEnergyImp(const InterpolationObject *embed, const double rho, const double max_rho);

public:
  /**
   * different eam potential style: eam/alloy, eam/fs.
   */
  EamPotTableLoaderApi *eam_pot_loader;

private:
  const atom_type::_type_atom_types _n_eles; // the count of element types, which is initialized as 0.
  // all kinds of atoms using the same cutoff.
  char latticeType[8]; //!< 晶格类型  @deprecated

public:
  /**
   * @param eam_style the eam style: can be eam/alloy or eam/fs.
   */
  const int eam_style = EAM_STYLE_ALLOY;
};

#endif // CRYSTAL_MD_EAM_H
