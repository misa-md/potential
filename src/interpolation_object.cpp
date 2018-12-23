#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include <algorithm>

#include "interpolation_object.h"
#include "utils.h"

InterpolationObject::InterpolationObject() {
    values = nullptr;
}

InterpolationObject::~InterpolationObject() {
    delete[] values;
    delete[] spline;
}

void InterpolationObject::initInterpolationObject(int _n, double _x0, double dx, double data[]) {
    n = _n;
    values = new double[n + 1];
    invDx = 1.0 / dx;
    x0 = _x0;
    for (int ii = 0; ii < n; ++ii) {
        values[ii + 1] = data[ii];
    }
}

void InterpolationObject::bcastInterpolationObject(const int root, const int rank, MPI_Comm comm) {
    typedef struct {
        int n;
        double x0;
        double inv_dx;
    } interpolation_data_pack;
    interpolation_data_pack temp;
    if (rank == root) {
        temp.n = this->n;
        temp.x0 = this->x0;
        temp.inv_dx = this->invDx;
    }

    MPI_Bcast(&temp, sizeof(interpolation_data_pack), MPI_BYTE, root, comm);

    if (rank != root) {
        this->n = temp.n;
        this->x0 = temp.x0;
        this->invDx = temp.inv_dx;
        values = new double[n + 1];
    }

    MPI_Bcast(values, n + 1, MPI_DOUBLE, 0, comm);
}

/**
 * @see https://github.com/lammps/lammps/blob/stable_16Mar2018/src/MANYBODY/pair_eam.cpp#L745
 * @see http://lammps.sandia.gov/threads/msg21496.html
 */
void InterpolationObject::interpolateFile() {
    spline = new double[n + 1][7];
    for (int m = 1; m <= n; m++) {
        spline[m][6] = values[m];
    }

    spline[1][5] = spline[2][6] - spline[1][6];
    spline[2][5] = 0.5 * (spline[3][6] - spline[1][6]);
    spline[n - 1][5] = 0.5 * (spline[n][6] - spline[n - 2][6]);
    spline[n][5] = spline[n][6] - spline[n - 1][6];

    for (int m = 3; m <= n - 2; m++) {
        spline[m][5] = ((spline[m - 2][6] - spline[m + 2][6]) +
                        8.0 * (spline[m + 1][6] - spline[m - 1][6])) / 12.0;
    }

    for (int m = 1; m <= n - 1; m++) {
        spline[m][4] = 3.0 * (spline[m + 1][6] - spline[m][6]) -
                       2.0 * spline[m][5] - spline[m + 1][5];
        spline[m][3] = spline[m][5] + spline[m + 1][5] -
                       2.0 * (spline[m + 1][6] - spline[m][6]);
    }

    spline[n][4] = 0.0;
    spline[n][3] = 0.0;

    for (int m = 1; m <= n; m++) {
        spline[m][2] = spline[m][5] * invDx;
        spline[m][1] = 2.0 * spline[m][4] * invDx;
        spline[m][0] = 3.0 * spline[m][3] * invDx;
    }
}
