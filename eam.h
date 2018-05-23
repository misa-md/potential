//
// Created by genshen on 2018-05-21.
//

#ifndef CRYSTAL_MD_EAM_H
#define CRYSTAL_MD_EAM_H

#include "eam_phi.h"
#include "eam_one_way.h"

class eam {
public:

    EamPhiList eam_phi; // pair potentials for N types elements.

    OneWayEamList electron_density;  //!< 电子云密度
    OneWayEamList embedded;    //!< 嵌入能

    eam();

    ~eam();

    /**
     * initialize elements count and initialize potential array with size {@var n_ele} types elements.
     * @param n_ele
     */
    void initElementN(_type_atom_types n_ele);

    void eamBCast(int rank);

    void interpolateFile();

    double toForce(atom_type::atom_type type_from, atom_type::atom_type type_to,
                   double dist2, double df_sum);

    /**
     * compute the contribution to electron charge density from atom j of type {@var _atom_type} at location of one atom i.
     * whose distance is specified by {@var dist2}
     * @param _atom_type atom type of atom j.
     * @param dist2 the square of the distance between atom i and atom j.
     * @return the contribution to electron charge density from atom j.
     */
    double rhoContribution(atom_type::atom_type _atom_type, double dist2);

    /**
     * compute embedded energy of atom of type {@var _atom_type},
     * whose electron charge density contributed by its neighbor atoms is specified by {@var rho}.
     * @param _atom_type atom type
     * @param rho  electron charge density contributed by all its neighbor atoms.
     * @return embedded energy of this atom.
     */
    double embedEnergyContribution(atom_type::atom_type _atom_type, double rho);

    /*del*/
    void setatomicNo(double nAtomic);

    void setlat(double latticeconst);

    void setmass(int i, double _mass);

    void setlatticeType(char *_latticeType);

    void setname(char *_name);

    void setcutoff(double _cutoff);
    /*/del*/

private:
    bool has_initialized;
    _type_atom_types _nElems; // the count of element types, which is initialized as 0.
    // all kinds of atoms using the same cutoff.
    double cutoff;          //!< 截断半径
    double *mass;           //!< 质量
    double lat;             //!< 晶格常数
    char latticeType[8];    //!< 晶格类型
    char name[3];       //!< 元素名
    int atomicNo;       //!< 元素序号
};

#endif //CRYSTAL_MD_EAM_H
