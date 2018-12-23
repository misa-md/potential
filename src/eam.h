//
// Created by genshen on 2018-05-21.
//

#ifndef CRYSTAL_MD_EAM_H
#define CRYSTAL_MD_EAM_H

#include "eam_phi.h"
#include "eam_one_way.h"
#include "types.h"

class eam {
    friend class SetflParser; // todo better permission.

public:
    EamPhiList eam_phi; // pair potentials for N types elements.

    OneWayEamList electron_density;  //!< 电子云密度
    OneWayEamList embedded;    //!< 嵌入能

    /**
     * create a new eam instance by the elements count from root processor with size {@var n_ele} types elements.
     * @param n_ele atom elements count, only rank @param root is correct(other processors are zero values).
     * @param root the root processor which has the correct element count @param n_ele.
     * @param rank current processor rank.
     * @param comm mpi communicator.
     * @return eam instance.
     */
    static eam *newInstance(atom_type::_type_atom_types n_ele,
                            const int root, const int rank, MPI_Comm comm);

    /**
     * initialize eam object with count/size of atom elements.
     * In the constructor, member @var eam_phi, @var electron_density and @var embedded will be initialized by @param n_ele.
     * @param n_ele the count of elements.
     */
    explicit eam(const atom_type::_type_atom_types n_ele);

    void eamBCast(const int root, const int rank, MPI_Comm comm);

    void interpolateFile();

    double toForce(const atom_type::_type_prop_key key_from, const atom_type::_type_prop_key key_to,
                   const double dist2, const double df_sum);

    /**
     * compute the contribution to electron charge density from atom j of type {@var _atom_type} at location of one atom i.
     * whose distance is specified by {@var dist2}
     * @param _atom_key atom type of atom j.
     * @param dist2 the square of the distance between atom i and atom j.
     * @return the contribution to electron charge density from atom j.
     */
    double rhoContribution(const atom_type::_type_prop_key _atom_key, const double dist2);

    /**
     * compute embedded energy of atom of type {@var _atom_type},
     * whose electron charge density contributed by its neighbor atoms is specified by {@var rho}.
     * @param _atom_key atom type
     * @param rho  electron charge density contributed by all its neighbor atoms.
     * @return embedded energy of this atom.
     */
    double embedEnergyContribution(const atom_type::_type_prop_key _atom_key, const double rho);

    /**
     * @deprecated
     * @param _latticeType
     */
    void setlatticeType(char *_latticeType);

private:
    const atom_type::_type_atom_types _nElems; // the count of element types, which is initialized as 0.
    // all kinds of atoms using the same cutoff.
    char latticeType[8];    //!< 晶格类型  @deprecated
};

#endif //CRYSTAL_MD_EAM_H
