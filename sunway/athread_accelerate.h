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
extern "C" void SLAVE_FUN(init_spline)(double **);

inline void athreadAccelerateInit(int &lolocalx, int &lolocaly, int &lolocalz,
                                  int &nlocalx, int &nlocaly, int &nlocalz,
                                  int &loghostx, int &loghosty, int &loghostz,
                                  int &nghostx, int &nghosty, int &nghostz);

inline void initSpline(double (*rhoSpline)[7], double (*fSpline)[7], double (*phiSpline)[7]);

inline void athreadAccelerateEamRhoCalc(int *rho_n, double *x, double *rho, double *cutoffRadius,
                                        double *rhoInvDx, double *rhoSplineValues);

inline void athreadAccelerateEamDfCalc(int *df_n, double *rho, double *df, double *cutoffRadius,
                                       double *dfSplineInvDx, double *dfSplineValues);

inline void athreadAccelerateEamForceCalc(int *phi_n, double *x, double *f, double *df,
                                          double *cutoffRadius, double *phiSplineInvDx,
                                          double *phiSplineValues, double *rhoSplineValues);

#endif //CRYSTAL_MD_ATHREAD_ACCELERATE_H
