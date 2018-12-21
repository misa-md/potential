//
// Created by baihe back to 2015-12-26.
//
#ifndef CRYSTAL_MD_INTERPOLATION_OBJECT_H
#define CRYSTAL_MD_INTERPOLATION_OBJECT_H

class InterpolationObject {
public:
    InterpolationObject();

    ~InterpolationObject();

    /**
     *
     * @param _n data count
     * @param _x0 start point
     * @param dx dx
     * @param data data with size {@var n}
     */
    void initInterpolationObject(int _n, double _x0, double dx, double data[]);

    void bcastInterpolationObject(int rank);

    void interpolatefile();

    int n;          //!< 表中数据个数
    double x0;      //!< 起始点
    double invDx;   //!< 倒数
    double *values;
    double (*spline)[7];
};

#endif //CRYSTAL_MD_INTERPOLATION_OBJECT_H