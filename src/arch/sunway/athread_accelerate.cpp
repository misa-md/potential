//
// Created by genshen on 2018-3-5.
//

#include <cstdio>
#include "athread_accelerate.h"

void athreadAccelerateInit(const int &lolocalx, const int &lolocaly, const int &lolocalz,
                           const int &nlocalx, const int &nlocaly, const int &nlocalz,
                           const int &loghostx, const int &loghosty, const int &loghostz,
                           const int &nghostx, const int &nghosty, const int &nghost) {
    int func[9];
    func[0] = lolocalx - loghostx;
    func[1] = lolocaly - loghosty;
    func[2] = lolocalz - loghostz;
    func[3] = nlocalx;
    func[4] = nlocaly;
    func[5] = nlocalz;
    func[6] = nghostx;
    func[7] = nghosty;
    func[8] = athread_get_max_threads();
    //printf("init\n");
    __real_athread_spawn((void *) slave_init, func); // todo slave_init or init

    athread_join();
}

void initSpline(double (*rhoSpline)[7], double (*fSpline)[7], double (*phiSpline)[7]) {
    double (*spline[3])[7];
    spline[0] = rhoSpline;
    spline[1] = fSpline;
    spline[2] = phiSpline;
    __real_athread_spawn((void *) slave_init_spline, spline);
    athread_join();
}

void athreadAccelerateEamRhoCalc(int *rho_n, double *x, double *rho, double *cutoffRadius,
                                 double *rhoInvDx, double *rhoSplineValues) {
//    double rho_n = rho_spline->n;
    double *func[6];
    func[0] = x;
    func[1] = rho;
    func[2] = cutoffRadius;
    func[3] = reinterpret_cast<double *>(rho_n); // todo int to double.
    func[4] = rhoInvDx;
    func[5] = rhoSplineValues;
//    printf("rho1\n");
    __real_athread_spawn((void *) slave_cal_rho1, func);
    athread_join();
//    printf("rho2\n");
    __real_athread_spawn((void *) slave_cal_rho2, func);
    athread_join();
}

void athreadAccelerateEamDfCalc(int *df_n, double *rho, double *df, double *cutoffRadius,
                                double *dfSplineInvDx, double *dfSplineValues) {
//    double df_n;
//    df_n = f_spline->n;
    double *func2[6];
    func2[0] = rho;
    func2[1] = df;
    func2[2] = cutoffRadius;
    func2[3] = reinterpret_cast<double *>(df_n); // todo int to double
    func2[4] = dfSplineInvDx;
    func2[5] = dfSplineValues;
    __real_athread_spawn((void *) slave_cal_df, func2);
    athread_join();
}

void athreadAccelerateEamForceCalc(int *phi_n, double *x, double *f, double *df,
                                   double *cutoffRadius, double *phiSplineInvDx,
                                   double *phiSplineValues, double *rhoSplineValues) {
//    phi_spline->n;
    double *func3[8];
    func3[0] = x;
    func3[1] = f;
    func3[2] = df;
    func3[3] = cutoffRadius;// &
    func3[4] = reinterpret_cast<double *>(phi_n); // &todo int to double
    func3[5] = phiSplineInvDx; // &phi_spline->invDx;
    func3[6] = phiSplineValues; // phi_spline->values;
    func3[7] = rhoSplineValues; // rho_spline->values;
    __real_athread_spawn((void *) slave_cal_force1, func3);
    athread_join();
    __real_athread_spawn((void *) slave_cal_force2, func3);
    athread_join();
    __real_athread_spawn((void *) slave_cal_force3, func3);
    athread_join();
}