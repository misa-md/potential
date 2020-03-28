//
// Created by baihe back to 2015-12-26.
//
#ifndef CRYSTAL_MD_INTERPOLATION_OBJECT_H
#define CRYSTAL_MD_INTERPOLATION_OBJECT_H

#include <mpi.h>

struct SplineData {
  double(*spline); // spline (it is a 1d array with length 7.)
  double p;        // x value in ax^3+bx^2+c for calculating interpolation result.
};

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

  /**
   * given a real value on x axis, find the corresponding spline and value p.
   */
  inline SplineData findSpline(const double value) const {
    double p = value * invDx + 1.0;
    int m = static_cast<int>(p);
    m = std::max(1, std::min(m, (n - 1)));
    p -= m;
    p = std::min(p, 1.0);
    return SplineData{spline[m], p};
  }

  void interpolateFile();

  int n;        //!< 表中数据个数
  double x0;    //!< 起始点
  double invDx; //!< 倒数
  double *values;
  double (*spline)[7];
};

#endif // CRYSTAL_MD_INTERPOLATION_OBJECT_H