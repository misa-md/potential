//
// Created by genshen on 18-3-5.
//

#ifndef CRYSTAL_MD_ATHREAD_ACCELERATE_H
#define CRYSTAL_MD_ATHREAD_ACCELERATE_H

extern "C" {
#include <athread.h>
}

// todo use c++ style.
extern "C" void SLAVE_FUN(init)(int *);
extern "C" void SLAVE_FUN(cal_rho1)(double *);
extern "C" void SLAVE_FUN(cal_rho2)(double *);
extern "C" void SLAVE_FUN(cal_df)(double *);
extern "C" void SLAVE_FUN(cal_force1)(double *);
extern "C" void SLAVE_FUN(cal_force2)(double *);
extern "C" void SLAVE_FUN(cal_force3)(double *);
extern "C" void SLAVE_FUN(init_spline)(double **);

void athreadAccelerateInit(const int &lolocalx, const int &lolocaly, const int &lolocalz,
                           const int &nlocalx, const int &nlocaly, const int &nlocalz,
                           const int &loghostx, const int &loghosty, const int &loghostz,
                           const int &nghostx, const int &nghosty, const int &nghostz);

void initSpline(double (*rhoSpline)[7], double (*fSpline)[7], double (*phiSpline)[7]);

void athreadAccelerateEamRhoCalc(int *rho_n, double *x, double *rho, double *cutoffRadius,
                                 double *rhoInvDx, double *rhoSplineValues);

void athreadAccelerateEamDfCalc(int *df_n, double *rho, double *df, double *cutoffRadius,
                                double *dfSplineInvDx, double *dfSplineValues);

void athreadAccelerateEamForceCalc(int *phi_n, double *x, double *f, double *df,
                                   double *cutoffRadius, double *phiSplineInvDx,
                                   double *phiSplineValues, double *rhoSplineValues);

#endif //CRYSTAL_MD_ATHREAD_ACCELERATE_H
