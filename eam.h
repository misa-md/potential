//
// Created by genshen on 2018-05-21.
//

#ifndef CRYSTAL_MD_EAM_H
#define CRYSTAL_MD_EAM_H

#include "eam_hpi.h"
class H{

};
class eam {
public:

    EamPhiList eam_phi; // pair potentials for N  types elements.

    InterpolationObject *phi;  //!< 对势
    InterpolationObject *rho;  //!< 电子云密度
    InterpolationObject *f;    //!< 嵌入能

    eam();

    ~eam();

    void init(int nElems);

    void eamBCast(int rank);

    void interpolateFile();

    void initf(int i, int nRho, double x0, double dRho, double *buf);

    void initphi(int i, int nR, double x0, double dR, double *buf) ;

    void initrho(int i, int nR, double x0, double dR, double *buf) ;

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
    int _nElems; // the count of element types, which is initialized as 0.
    // all kinds of atoms using the same cutoff.
    double cutoff;          //!< 截断半径
    double *mass;           //!< 质量
    double lat;             //!< 晶格常数
    char latticeType[8];    //!< 晶格类型
    char name[3];       //!< 元素名
    int atomicNo;       //!< 元素序号
};

#endif //CRYSTAL_MD_EAM_H
