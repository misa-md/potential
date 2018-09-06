#include <cstdlib>
#include <mpi.h>
#include <iostream>
#include <algorithm>
#include <utils/bundle.h>
#include <utils/mpi_utils.h>

#include "interpolation_object.h"
#include "../utils/mpi_domain.h"
#include "../types/pre_define.h"

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

void InterpolationObject::bcastInterpolationObject(int rank) {
    kiwi::Bundle bundle = kiwi::Bundle();
    bundle.newPackBuffer(sizeof(int) + 2 * sizeof(double));

    if (rank == MASTER_PROCESSOR) {
        bundle.put(n);
        bundle.put(x0);
        bundle.put(invDx);
    }
    MPI_Bcast(bundle.getPackedData(), bundle.getPackedDataCap(), MPI_BYTE,
              MASTER_PROCESSOR, MPIDomain::sim_processor.comm);
    if (rank != MASTER_PROCESSOR) {
        int cursor = 0;
        bundle.get(cursor, n);
        bundle.get(cursor, x0);
        bundle.get(cursor, invDx);
        values = new double[n + 1];
    }
    bundle.freePackBuffer();
    MPI_Bcast(values, n + 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
}

/**
 * @see https://github.com/lammps/lammps/blob/stable_16Mar2018/src/MANYBODY/pair_eam.cpp#L745
 * @see http://lammps.sandia.gov/threads/msg21496.html
 */
void InterpolationObject::interpolatefile() {
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
