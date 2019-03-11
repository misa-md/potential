//
// Created by baihe back to 2015-12-26.
//
#ifndef CRYSTAL_MD_INTERPOLATION_OBJECT_H
#define CRYSTAL_MD_INTERPOLATION_OBJECT_H

#include <mpi.h>

class InterpolationObject {
public:
    InterpolationObject();

    ~InterpolationObject();

    /**
     * initialize data
     * @param _n the length of array @param data.
     * @param _x0 start point
     * @param dx dx
     * @param data data with size {@param _n}
     */
    void initInterpolationObject(const int _n, const double _x0, const double dx, double data[]);

    void bcastInterpolationObject(const int root, const int rank, MPI_Comm comm);

    void interpolateFile();

    int n;          //!< 表中数据个数
    double x0;      //!< 起始点
    double invDx;   //!< 倒数
    double *values;
    double (*spline)[7];
};

#endif //CRYSTAL_MD_INTERPOLATION_OBJECT_H