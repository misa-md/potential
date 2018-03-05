/*
 * those runs on sunway SW26010 architecture, with its slave core.
 * For accelerating calculate.
 */
#include "slave.h"
#include <math.h>
#include <time.h>

__thread_local volatile unsigned long reply, _reply[2], put_reply;
__thread_local int my_id;
__thread_local int Nvalue;
__thread_local int xstart, ystart, zstart;
__thread_local int nlocalx, nlocaly, nlocalz;
__thread_local int nghostx, nghosty;
__thread_local int NeighborOffset[68];
__thread_local double (*rho_spline)[7], (*f_spline)[7], (*phi_spline)[7];
__thread_local double *_spline_;

#define max(x, y) (x>y?x:y)
#define min(x, y) (x>y?y:x)

void rtc(unsigned long *counter);

int BeginOfArea(int natom, int start, int subindex);

int EndOfArea(int natom, int start, int subindex);

long int IndexOf3DIndex(long int xIndex, long int yIndex, long int zIndex);

long int co_IndexOf3DIndex(long int xIndex, long int yIndex, long int zIndex);

void calculateNeighbourIndices();

void init(int *a) {
    int i, j;
    _spline_ = (double *) ldm_malloc(sizeof(double) * (5000));
    my_id = get_row() * 8 + get_col();
    xstart = a[0];
    ystart = a[1];
    zstart = a[2];
    nlocalx = a[3];
    nlocaly = a[4];
    nlocalz = a[5];
    nghostx = a[6];
    nghosty = a[7];
    Nvalue = a[8] * 2;
    calculateNeighbourIndices();
}

void calculateNeighbourIndices() {
    int i;
    double x, y, z;
    int xIndex, yIndex, zIndex;
    int index = 0;
    int mark = 0;
    double _cutoffRadius = 5.6;
    double _latticeconst = 2.85532;
    int _cutlattice = 2;
    double cut_times_latti = _cutoffRadius / _latticeconst;
    double square, r;
    long int offset;
    for (zIndex = -_cutlattice; zIndex <= _cutlattice; zIndex++) {
        for (yIndex = -_cutlattice; yIndex <= _cutlattice; yIndex++) {
            for (xIndex = -_cutlattice * 2; xIndex <= _cutlattice * 2; xIndex++) {
                z = (double) zIndex + (((double) (xIndex % 2)) / 2);
                y = (double) yIndex + (((double) (xIndex % 2)) / 2);
                x = (double) xIndex / 2;
                square = x * x + y * y + z * z;
                offset;
                r = sqrt(square);
                if (r < (cut_times_latti + 0.4)) {
                    offset = co_IndexOf3DIndex(xIndex, yIndex, zIndex);
                    if (offset > 0) {
                        NeighborOffset[index] = offset;
                        index++;
                    }
                }
                z = (double) zIndex - (((double) (xIndex % 2)) / 2);
                y = (double) yIndex - (((double) (xIndex % 2)) / 2);
                x = (double) xIndex / 2;
                square = x * x + y * y + z * z;
                r = sqrt(square);
                if (r < (cut_times_latti + 0.4)) {
                    offset = co_IndexOf3DIndex(xIndex, yIndex, zIndex);
                    if (offset > 0) {
                        for (i = 0; i < index; i++) {
                            if (NeighborOffset[i] == offset) {
                                mark = 1;
                                break;
                            }
                        }
                        if (mark != 1) {
                            NeighborOffset[index] = offset;
                            index++;
                        }
                        mark = 0;
                    }
                }
            }
        }
    }
}

long int co_IndexOf3DIndex(long int xIndex, long int yIndex, long int zIndex) {
    return (zIndex * 5 + yIndex) * 16 + xIndex;
}

void init_spline(double **spline[]) {
    rho_spline = spline[0];
    f_spline = spline[1];
    phi_spline = spline[2];
}

void cal_rho1(double *a[]) {
    int subindex_z, zOmp_start, zOmp_end;
    int i, j, k, l, m_block, kk;
    double cutoffRadius;
    double xtemp, ytemp, ztemp;
    double delx, dely, delz;
    int n;
    double *x, *rho, (*spline)[7], *spline_mem;
    double *value;
    double _spline[4];
    int rho_n;
    double invDx;
    double dist2, r, p, rhoTmp;
    int m;
    double _x[16 * 15 * 3], _rho[16 * 15];
    int m_step, x_end;
    int n_block;
    int next;
    unsigned long start, stop;
    //printf("space:%d \n", get_allocatable_size());
    //_x = (double*)ldm_malloc(sizeof(double)*(16*15*3));
    //_rho = (double*)ldm_malloc(sizeof(double)*(16*15));
    //_spline = (double*)ldm_malloc(sizeof(double)*4);

    x = a[0];
    rho = a[1];
    cutoffRadius = *a[2];
    rho_n = *a[3];
    invDx = *a[4];
    value = a[5];
    subindex_z = (my_id * 2) % Nvalue;
    spline = rho_spline;
    spline_mem = spline + 7;
    zOmp_start = BeginOfArea(nlocalz, zstart, subindex_z);
    zOmp_end = EndOfArea(nlocalz, zstart, subindex_z);
    m_step = 0;
    n_block = (nlocalx % 8) ? (nlocalx / 8 + 1) : (nlocalx / 8);

    reply = 0;
    athread_get(PE_MODE, &value[0], &_spline_[0], 5000 * 8, &reply, 0, 0, 0);
    while (reply != 1);
    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            //stoptime = clock();
            //time += difftime(stoptime,starttime)/CLOCKS_PER_SEC;
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[0], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[16 * 5], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[16 * 10], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 6);
                //athread_syn(ARRAY_SCOPE, 0xffff);
                for (i = 0; i < x_end; i++) {
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //printf("x;%d, %lf, %lf, %lf\n",kk, xtemp, ytemp, ztemp);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            //printf("nei:%lf, %lf, %lf\n", _x[n*3], _x[n*3+1], _x[n*3+2]);
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                //rtc(&start);
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (rho_n - 1)));

                                //spline_mem = spline[m]+3;
                                //rtc(&start);
                                //spline_mem = spline+m*7+3;
                                //rtc(&stop);
                                //rtc(&start);
                                //reply = 0;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][3], &_spline[0], 4*8, &reply, 0, 0, 0);
                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);
                                _spline[3] = _spline_[m];
                                _spline[2] = ((_spline_[m - 2] - _spline_[m + 2]) +
                                              8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0;
                                _spline[1] = 3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * _spline[2] -
                                             ((_spline_[m - 1] - _spline_[m + 3]) +
                                              8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0;
                                _spline[0] = _spline[2] + ((_spline_[m - 1] - _spline_[m + 3]) +
                                                           8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0 -
                                             2.0 * (_spline_[m + 1] - _spline_[m]);
                                //rtc(&stop);
                                rhoTmp = ((_spline[0] * p + _spline[1]) * p + _spline[2]) * p + _spline[3];
                                //printf("tmp:%lf\n", rhoTmp);
                                //rhoTmp = ((spline[m][3]*p + spline[m][4])*p + spline[m][5])*p + spline[m][6];
                                //printf("tmp:%lf\n", rhoTmp);
                                _rho[kk] += rhoTmp;
                                _rho[n] += rhoTmp;
                                //rtc(&stop);
                                //if(my_id == 0)
                                //printf("r:%d\n", stop-start);
                                //printf("tmp:%lf, %lf, %lf, %lf\n", _spline[0], _spline[1], _spline[2], rhoTmp);
                            }
                        }
                    }
                    //printf("rho:%lf\n", _rho_put[kk]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                //printf("kk_after:%d, %lf, %lf\n", kk, _rho_put[36], _rho_put[52]);
                //printf("kk:%d, %d, %d, %d\n", kk+nghostx, 50*(m_block%n_block), j, k
                athread_put(PE_MODE, &_rho[0], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_rho[16 * 5], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_rho[16 * 10], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                while (put_reply != 3);
            }
        }
    }
    while (put_reply != 3);
    //printf("a");
    //ldm_free(_x, sizeof(double)*(16*15*3));
    //ldm_free(_rho, sizeof(double)*(16*15));
    //ldm_free(_spline, sizeof(double)*4);
}

void cal_rho2(double *a[]) {
    int subindex_z, zOmp_start, zOmp_end;
    int i, j, k, l, kk, m_block;
    double cutoffRadius;
    double xtemp, ytemp, ztemp;
    double delx, dely, delz;
    int n;
    double *x, *rho, (*spline)[7], *spline_mem;
    int rho_n;
    double invDx;
    int m;
    double dist2, r, p, rhoTmp;
    double _spline[4];
    double _x[16 * 15 * 3], _rho[16 * 15];
    int m_step, x_end;
    int n_block;
    int next;
    //if(my_id == 0)
    //	printf("s\n");
    //_x = (double*)ldm_malloc(sizeof(double)*(16*15*3));
    //_rho = (double*)ldm_malloc(sizeof(double)*(16*15));
    //_spline = (double*)ldm_malloc(sizeof(double)*4);
    //if(my_id == 0)
    //	printf("space:%d \n", get_allocatable_size());
    //athread_syn(ARRAY_SCOPE, 0xffff);
    x = a[0];
    rho = a[1];
    cutoffRadius = *a[2];
    rho_n = *a[3];
    invDx = *a[4];
    spline = rho_spline;
    m_step = 0;
    n_block = (nlocalx % 8) ? (nlocalx / 8 + 1) : (nlocalx / 8);

    subindex_z = (my_id * 2 + 1) % Nvalue;
    zOmp_start = BeginOfArea(nlocalz, zstart, subindex_z);
    zOmp_end = EndOfArea(nlocalz, zstart, subindex_z);
    if (my_id == (Nvalue / 2 - 1)) {
        zOmp_end = nlocalz + zstart;
    }


    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[0], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[16 * 5], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &rho[kk], &_rho[16 * 10], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 6);
                for (i = 0; i < x_end; i++) {
                    //printf("before:%d, %d, %lf, %lf\n", next, *_reply_comp, _rho_put[kk], rho[55070+nghostx*2+4]);
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //	printf("%d, %d, %lf, %lf, %lf, %lf\n", zOmp_start, _reply[0], xtemp, ytemp, ztemp, _rho_put[kk]);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (rho_n - 1)));
                                //reply = 0;
                                //asm volatile ("memb");
                                //spline_mem = spline+m*7+3;
                                //athread_get(PE_MODE, &spline[m][3], &_spline[0], 4*8, &reply, 0, 0, 0);
                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);
                                _spline[3] = _spline_[m];
                                _spline[2] = ((_spline_[m - 2] - _spline_[m + 2]) +
                                              8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0;
                                _spline[1] = 3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * _spline[2] -
                                             ((_spline_[m - 1] - _spline_[m + 3]) +
                                              8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0;
                                _spline[0] = _spline[2] + ((_spline_[m - 1] - _spline_[m + 3]) +
                                                           8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0 -
                                             2.0 * (_spline_[m + 1] - _spline_[m]);
                                rhoTmp = ((_spline[0] * p + _spline[1]) * p + _spline[2]) * p + _spline[3];
                                _rho[kk] += rhoTmp;
                                _rho[n] += rhoTmp;
                            }
                        }
                    }
                    //if(my_id == 0)
                    //printf("%lf\n", _rho_put[kk]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_put(PE_MODE, &_rho[0], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_rho[16 * 5], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_rho[16 * 10], &rho[kk], 16 * 5 * 8, &put_reply, (nghostx - 16) * 8, 16 * 8);
                while (put_reply != 3);
            }
        }
    }
    //printf("b");
    //athread_syn(ARRAY_SCOPE, 0xffff);
    //ldm_free(_x, sizeof(double)*(16*15*3));
    //ldm_free(_rho, sizeof(double)*(16*15));
    //ldm_free(_spline, sizeof(double)*4);
}

void cal_df(double *a[]) {
    int zOmp_start, zOmp_end;
    int i, j, k, kk;
    double cutoffRadius;
    double *rho, *df, (*spline)[7];
    double *value;
    int df_n;
    double invDx;
    int m;
    double dist2, p, dfEmbed;
    double *_rho, *_df;
    double *_spline;
    //double *_rho, *_df;
    //_rho = (double*)ldm_malloc(sizeof(double)*(nlocalx));
    //_df = (double*)ldm_malloc(sizeof(double)*(nghostx));
    _rho = (double *) ldm_malloc(sizeof(double) * (nlocalx));
    _df = (double *) ldm_malloc(sizeof(double) * (nlocalx));
    _spline = (double *) ldm_malloc(sizeof(double) * 3);
    //athread_syn(ARRAY_SCOPE, 0xffff);
    rho = a[0];
    df = a[1];
    cutoffRadius = *a[2];
    df_n = *a[3];
    invDx = *a[4];
    value = a[5];
    spline = f_spline;
    zOmp_start = zstart + ((nlocalz / (Nvalue / 2)) * my_id);
    zOmp_end = zstart + ((nlocalz / (Nvalue / 2)) * (my_id + 1));

    reply = 0;
    athread_get(PE_MODE, &value[0], &_spline_[0], 5000 * 8, &reply, 0, 0, 0);
    while (reply != 1);

    if (my_id == (Nvalue / 2 - 1)) {
        zOmp_end = nlocalz + zstart;
    }
    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            reply = 0;
            kk = IndexOf3DIndex(xstart, j, k);
            athread_get(PE_MODE, &rho[kk], &_rho[0], nlocalx * 8, &reply, 0, 0, 0);

            while (reply != 1);
            kk = 0;
            for (i = xstart; i < nlocalx + xstart; i++) {
                p = _rho[kk] * invDx + 1.0;
                m = (int) p;
                m = max(1, min(m, (df_n - 1)));
                //reply = 0;
                //asm volatile ("memb");
                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 3*8, &reply, 0, 0, 0);
                p -= m;
                p = min(p, 1.0);
                //while(reply!=1);
                _spline[2] =
                        (((_spline_[m - 2] - _spline_[m + 2]) + 8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) *
                        invDx;
                _spline[1] = 2.0 * (3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 *
                                                                            (((_spline_[m - 2] - _spline_[m + 2]) +
                                                                              8.0 *
                                                                              (_spline_[m + 1] - _spline_[m - 1])) /
                                                                             12.0) -
                                    (((_spline_[m - 1] - _spline_[m + 3]) + 8.0 * (_spline_[m + 2] - _spline_[m])) /
                                     12.0)) * invDx;
                _spline[0] = 3.0 * ((((_spline_[m - 2] - _spline_[m + 2]) + 8.0 * (_spline_[m + 1] - _spline_[m - 1])) /
                                     12.0) +
                                    (((_spline_[m - 1] - _spline_[m + 3]) + 8.0 * (_spline_[m + 2] - _spline_[m])) /
                                     12.0) - 2.0 * (_spline_[m + 1] - _spline_[m])) * invDx;
                dfEmbed = (_spline[0] * p + _spline[1]) * p + _spline[2];
                //dfEmbed = (spline[m][0]*p + spline[m][1])*p + spline[m][2];

                _df[kk] = dfEmbed;
                kk++;
                //if(i ==6 && j == 3 && k == 463)
                //if(dfEmbed > 1.0)
                //	printf("slave:%d, %d, %lf, %lf, %lf, %lf", my_id, zOmp_start, _spline[0], _spline[1], _spline[2], dfEmbed);
            }
            put_reply = 0;
            kk = IndexOf3DIndex(xstart, j, k);
            athread_put(PE_MODE, &_df[0], &df[kk], nlocalx * 8, &put_reply, 0, 0);
            while (put_reply != 1);
        }
    }
    //athread_syn(ARRAY_SCOPE, 0xffff);
    ldm_free(_rho, sizeof(double) * (nlocalx));
    ldm_free(_df, sizeof(double) * (nlocalx));
    ldm_free(_spline, sizeof(double) * 3);
}

void cal_force1(double *a[]) {
    int subindex_z, zOmp_start, zOmp_end;
    int i, j, k, l, m_block, kk;
    double cutoffRadius;
    double xtemp, ytemp, ztemp;
    double delx, dely, delz;
    int n;
    double *x, *f, *df, (*spline)[7];
    double *value;
    int phi_n;
    double invDx;
    int m;
    double dist2, r, p, phiTmp, dPhi, dRho;
    double z2, z2p, recip, phi, phip, psip, fpair;
    double *_x, *_f, *_df;
    double *_spline;
    int m_step, x_end;
    int n_block;
    //double *_x, *_f, *_df;
    //_x = (double*)ldm_malloc(sizeof(double)*(nghostx*15*3));
    //_f = (double*)ldm_malloc(sizeof(double)*(nghostx*15*3));
    //_df = (double*)ldm_malloc(sizeof(double)*(nghostx*15));
    _x = (double *) ldm_malloc(sizeof(double) * (16 * 15 * 3));
    _f = (double *) ldm_malloc(sizeof(double) * (16 * 15 * 3));
    _df = (double *) ldm_malloc(sizeof(double) * (16 * 15));
    _spline = (double *) ldm_malloc(sizeof(double) * 7);

    x = a[0];
    f = a[1];
    df = a[2];
    cutoffRadius = *a[3];
    phi_n = *a[4];
    invDx = *a[5];
    value = a[6];
    subindex_z = (my_id * 2) % Nvalue;
    zOmp_start = BeginOfArea(nlocalz, zstart, subindex_z);
    zOmp_end = EndOfArea(nlocalz, zstart, subindex_z);
    m_step = 0;
    n_block = (nlocalx % 8) ? (nlocalx / 8 + 1) : (nlocalx / 8);

    reply = 0;
    athread_get(PE_MODE, &value[0], &_spline_[0], 5000 * 8, &reply, 0, 0, 0);
    while (reply != 1);

    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[0], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[16*5], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[16*10], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 6);
                for (i = 0; i < x_end; i++) {
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //	printf("x:%lf, %lf, %lf\n", xtemp, ytemp, ztemp);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (phi_n - 1)));
                                //reply = 0;
                                //spline = phi_spline;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 7*8, &reply, 0, 0, 0);

                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);
                                _spline[6] = _spline_[m];
                                _spline[5] = ((_spline_[m - 2] - _spline_[m + 2]) +
                                              8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0;
                                _spline[4] = 3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * _spline[5] -
                                             ((_spline_[m - 1] - _spline_[m + 3]) +
                                              8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0;
                                _spline[3] = _spline[5] + ((_spline_[m - 1] - _spline_[m + 3]) +
                                                           8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0 -
                                             2.0 * (_spline_[m + 1] - _spline_[m]);
                                _spline[2] = (((_spline_[m - 2] - _spline_[m + 2]) +
                                               8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) * invDx;
                                _spline[1] = 2.0 * (3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * (((_spline_[m - 2] -
                                                                                                     _spline_[m + 2]) +
                                                                                                    8.0 *
                                                                                                    (_spline_[m + 1] -
                                                                                                     _spline_[m - 1])) /
                                                                                                   12.0) -
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0)) * invDx;
                                _spline[0] = 3.0 * ((((_spline_[m - 2] - _spline_[m + 2]) +
                                                      8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) +
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0) -
                                                    2.0 * (_spline_[m + 1] - _spline_[m])) * invDx;
                                phiTmp = ((_spline[3] * p + _spline[4]) * p + _spline[5]) * p + _spline[6];
                                //phiTmp = ((spline[m][3]*p + spline[m][4])*p + spline[m][5])*p + spline[m][6];
                                dPhi = (_spline[0] * p + _spline[1]) * p + _spline[2];
                                //dPhi = (spline[m][0]*p + spline[m][1])*p + spline[m][2];
                                //dRho = (spline[m][0]*p + spline[m][1])*p + spline[m][2];

                                z2 = phiTmp;
                                z2p = dPhi;
                                recip = 1.0 / r;
                                phi = z2 * recip;
                                phip = z2p * recip - phi * recip;
                                psip = phip;
                                fpair = -psip * recip;
                                //printf("pair:%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %.15lf\n", m, _spline[2], z2, z2p, _df[kk], _x[n * 3], _x[n * 3+1], _x[n * 3+2], fpair);

                                _f[kk * 3] += delx * fpair;
                                _f[kk * 3 + 1] += dely * fpair;
                                _f[kk * 3 + 2] += delz * fpair;

                                _f[n * 3] -= delx * fpair;
                                _f[n * 3 + 1] -= dely * fpair;
                                _f[n * 3 + 2] -= delz * fpair;
                                //if(my_id == 0)
                                //	printf("tmp:%lf, %lf, %lf, %.18lf\n", z2, z2p, psip, fpair);
                                //if(kk==(nghostx*2+6+3))
                                //	printf("kk+:%.15lf\n", _f[kk*3]);
                                //if(n==(nghostx*2+6+3))
                                //       printf("n-:%.15lf\n", _f[n*3]);
                            }
                        }
                    }
                    //if(my_id == 0)
                    //	printf("f:%.15lf\n", _f_put[kk*3]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_put(PE_MODE, &_f[0], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_f[16 * 15], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_f[16 * 10 * 3], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                while (put_reply != 3);
            }
        }
    }

    value = a[7];
    reply = 0;
    athread_get(PE_MODE, &value[0], &_spline_[0], 5000 * 8, &reply, 0, 0, 0);
    while (reply != 1);

    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[0], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[16 * 5], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[16 * 10], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 9);
                for (i = 0; i < x_end; i++) {
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //	printf("x:%lf, %lf, %lf\n", xtemp, ytemp, ztemp);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (phi_n - 1)));
                                //reply = 0;
                                //spline = phi_spline;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 7*8, &reply, 0, 0, 0);

                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);

                                _spline[2] = (((_spline_[m - 2] - _spline_[m + 2]) +
                                               8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) * invDx;
                                _spline[1] = 2.0 * (3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * (((_spline_[m - 2] -
                                                                                                     _spline_[m + 2]) +
                                                                                                    8.0 *
                                                                                                    (_spline_[m + 1] -
                                                                                                     _spline_[m - 1])) /
                                                                                                   12.0) -
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0)) * invDx;
                                _spline[0] = 3.0 * ((((_spline_[m - 2] - _spline_[m + 2]) +
                                                      8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) +
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0) -
                                                    2.0 * (_spline_[m + 1] - _spline_[m])) * invDx;

                                dRho = (_spline[0] * p + _spline[1]) * p + _spline[2];
                                //dRho = (spline[m][0]*p + spline[m][1])*p + spline[m][2];

                                psip = (_df[kk] + _df[n]) * dRho;
                                fpair = -psip * recip;
                                //printf("pair:%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %.15lf\n", m, _spline[2], z2, z2p, _df[kk], _x[n * 3], _x[n * 3+1], _x[n * 3+2], fpair);

                                _f[kk * 3] += delx * fpair;
                                _f[kk * 3 + 1] += dely * fpair;
                                _f[kk * 3 + 2] += delz * fpair;

                                _f[n * 3] -= delx * fpair;
                                _f[n * 3 + 1] -= dely * fpair;
                                _f[n * 3 + 2] -= delz * fpair;
                                //if(my_id == 0)
                                //	printf("tmp:%lf, %lf, %lf, %.18lf\n", z2, z2p, psip, fpair);
                                //if(kk==(nghostx*2+6+3))
                                //	printf("kk+:%.15lf\n", _f[kk*3]);
                                //if(n==(nghostx*2+6+3))
                                //       printf("n-:%.15lf\n", _f[n*3]);
                            }
                        }
                    }
                    //if(my_id == 0)
                    //	printf("f:%.15lf\n", _f_put[kk*3]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_put(PE_MODE, &_f[0], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_f[16 * 15], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_f[16 * 10 * 3], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                while (put_reply != 3);
            }
        }
    }

    ldm_free(_x, sizeof(double) * (16 * 15 * 3));
    ldm_free(_f, sizeof(double) * (16 * 15 * 3));
    ldm_free(_df, sizeof(double) * (16 * 15));
    ldm_free(_spline, sizeof(double) * 7);
}

void cal_force2(double *a[]) {
    int subindex_z, zOmp_start, zOmp_end;
    int i, j, k, l, kk, m_block;
    double cutoffRadius;
    double xtemp, ytemp, ztemp;
    double delx, dely, delz;
    int n;
    double *x, *f, *df, (*spline)[7];
    double *value;
    int phi_n;
    double invDx;
    int m;
    double dist2, r, p, phiTmp, dPhi, dRho;
    double z2, z2p, recip, phi, phip, psip, fpair;
    double *_x, *_f, *_df;
    double *_spline;
    int m_step, x_end;
    int n_block;
    //double *_x, *_f, *_df;
    //_x = (double*)ldm_malloc(sizeof(double)*(nghostx*15*3));
    //_f = (double*)ldm_malloc(sizeof(double)*(nghostx*15*3));
    //_df = (double*)ldm_malloc(sizeof(double)*(nghostx*15));
    _x = (double *) ldm_malloc(sizeof(double) * (16 * 15 * 3));
    _f = (double *) ldm_malloc(sizeof(double) * (16 * 15 * 3));
    _df = (double *) ldm_malloc(sizeof(double) * (16 * 15));
    _spline = (double *) ldm_malloc(sizeof(double) * 7);

    x = a[0];
    f = a[1];
    df = a[2];
    cutoffRadius = *a[3];
    phi_n = *a[4];
    invDx = *a[5];
    value = a[6];
    subindex_z = (my_id * 2 + 1) % Nvalue;
    zOmp_start = BeginOfArea(nlocalz, zstart, subindex_z);
    zOmp_end = EndOfArea(nlocalz, zstart, subindex_z);
    if (my_id == (Nvalue / 2 - 1)) {
        zOmp_end = nlocalz + zstart;
    }
    m_step = 0;
    n_block = (nlocalx % 8) ? (nlocalx / 8 + 1) : (nlocalx / 8);

    //reply = 0;
    //athread_get(PE_MODE, &value[0], &_spline_[0], 5000*8, &reply, 0, 0, 0);
    //while(reply!=1);

    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[0], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[16*5], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                //athread_get(PE_MODE, &df[kk], &_df[16*10], 16*5*8, &reply, 0, (nghostx-16)*8, 16*8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 6);
                for (i = 0; i < x_end; i++) {
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //	printf("x:%lf, %lf, %lf\n", xtemp, ytemp, ztemp);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (phi_n - 1)));
                                //reply = 0;
                                //spline = phi_spline;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 7*8, &reply, 0, 0, 0);

                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);
                                _spline[6] = _spline_[m];
                                _spline[5] = ((_spline_[m - 2] - _spline_[m + 2]) +
                                              8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0;
                                _spline[4] = 3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * _spline[5] -
                                             ((_spline_[m - 1] - _spline_[m + 3]) +
                                              8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0;
                                _spline[3] = _spline[5] + ((_spline_[m - 1] - _spline_[m + 3]) +
                                                           8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0 -
                                             2.0 * (_spline_[m + 1] - _spline_[m]);
                                _spline[2] = (((_spline_[m - 2] - _spline_[m + 2]) +
                                               8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) * invDx;
                                _spline[1] = 2.0 * (3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * (((_spline_[m - 2] -
                                                                                                     _spline_[m + 2]) +
                                                                                                    8.0 *
                                                                                                    (_spline_[m + 1] -
                                                                                                     _spline_[m - 1])) /
                                                                                                   12.0) -
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0)) * invDx;
                                _spline[0] = 3.0 * ((((_spline_[m - 2] - _spline_[m + 2]) +
                                                      8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) +
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0) -
                                                    2.0 * (_spline_[m + 1] - _spline_[m])) * invDx;
                                phiTmp = ((_spline[3] * p + _spline[4]) * p + _spline[5]) * p + _spline[6];
                                //phiTmp = ((spline[m][3]*p + spline[m][4])*p + spline[m][5])*p + spline[m][6];
                                dPhi = (_spline[0] * p + _spline[1]) * p + _spline[2];
                                //dPhi = (spline[m][0]*p + spline[m][1])*p + spline[m][2];
                                //reply = 0;
                                //spline = rho_spline;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 3*8, &reply, 0, 0, 0);
                                //while(reply!=1);
                                //dRho = (_spline[0]*p + _spline[1])*p + _spline[2];
                                //dRho = (spline[m][0]*p + spline[m][1])*p + spline[m][2];

                                z2 = phiTmp;
                                z2p = dPhi;
                                recip = 1.0 / r;
                                phi = z2 * recip;
                                phip = z2p * recip - phi * recip;
                                psip = phip;
                                //psip = (_df[kk] + _df[n])*dRho + phip;
                                fpair = -psip * recip;
                                //printf("pair:%lf, %lf, %lf, %.15lf\n", _x[n * 3], _x[n * 3+1], _x[n * 3+2], fpair);

                                _f[kk * 3] += delx * fpair;
                                _f[kk * 3 + 1] += dely * fpair;
                                _f[kk * 3 + 2] += delz * fpair;

                                _f[n * 3] -= delx * fpair;
                                _f[n * 3 + 1] -= dely * fpair;
                                _f[n * 3 + 2] -= delz * fpair;
                                //if(my_id == 0)
                                //	printf("tmp:%lf, %lf, %lf, %.18lf\n", z2, z2p, psip, fpair);
                                //if(kk==(nghostx*2+6+3))
                                //	printf("kk+:%.15lf\n", _f[kk*3]);
                                //if(n==(nghostx*2+6+3))
                                //       printf("n-:%.15lf\n", _f[n*3]);
                            }
                        }
                    }
                    //if(my_id == 0)
                    //	printf("f:%.15lf\n", _f_put[kk*3]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_put(PE_MODE, &_f[0], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_f[16 * 15], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_f[16 * 10 * 3], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                while (put_reply != 3);
            }
        }
    }

    value = a[7];
    reply = 0;
    athread_get(PE_MODE, &value[0], &_spline_[0], 5000 * 8, &reply, 0, 0, 0);
    while (reply != 1);

    for (k = zOmp_start; k < zOmp_end; k++) {
        for (j = ystart; j < nlocaly + ystart; j++) {
            for (m_step = 0; m_step < n_block; m_step++) {
                reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_get(PE_MODE, &x[kk * 3], &_x[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[0], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[0], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 5 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[16 * 5], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_get(PE_MODE, &x[kk * 3], &_x[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &f[kk * 3], &_f[16 * 10 * 3], 16 * 5 * 3 * 8, &reply, 0, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                athread_get(PE_MODE, &df[kk], &_df[16 * 10], 16 * 5 * 8, &reply, 0, (nghostx - 16) * 8, 16 * 8);

                kk = 16 * 2 + 4;
                if ((m_step % n_block) < n_block - 1)
                    x_end = 8;
                else
                    x_end = nlocalx - 8 * (n_block - 1);
                while (reply != 9);
                for (i = 0; i < x_end; i++) {
                    xtemp = _x[kk * 3];
                    ytemp = _x[kk * 3 + 1];
                    ztemp = _x[kk * 3 + 2];
                    //if(my_id == 0)
                    //	printf("x:%lf, %lf, %lf\n", xtemp, ytemp, ztemp);
                    if (xtemp != -100) {
                        for (l = 0; l < 68; l++) {
                            n = kk + NeighborOffset[l];
                            delx = xtemp - _x[n * 3];
                            dely = ytemp - _x[n * 3 + 1];
                            delz = ztemp - _x[n * 3 + 2];
                            dist2 = delx * delx + dely * dely + delz * delz;
                            if (dist2 < (cutoffRadius * cutoffRadius)) {
                                r = sqrt(dist2);
                                p = r * invDx + 1.0;
                                m = (int) p;
                                m = max(1, min(m, (phi_n - 1)));
                                //reply = 0;
                                //spline = phi_spline;
                                //asm volatile ("memb");
                                //athread_get(PE_MODE, &spline[m][0], &_spline[0], 7*8, &reply, 0, 0, 0);

                                p -= m;
                                p = min(p, 1.0);
                                //while(reply!=1);

                                _spline[2] = (((_spline_[m - 2] - _spline_[m + 2]) +
                                               8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) * invDx;
                                _spline[1] = 2.0 * (3.0 * (_spline_[m + 1] - _spline_[m]) - 2.0 * (((_spline_[m - 2] -
                                                                                                     _spline_[m + 2]) +
                                                                                                    8.0 *
                                                                                                    (_spline_[m + 1] -
                                                                                                     _spline_[m - 1])) /
                                                                                                   12.0) -
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0)) * invDx;
                                _spline[0] = 3.0 * ((((_spline_[m - 2] - _spline_[m + 2]) +
                                                      8.0 * (_spline_[m + 1] - _spline_[m - 1])) / 12.0) +
                                                    (((_spline_[m - 1] - _spline_[m + 3]) +
                                                      8.0 * (_spline_[m + 2] - _spline_[m])) / 12.0) -
                                                    2.0 * (_spline_[m + 1] - _spline_[m])) * invDx;

                                dRho = (_spline[0] * p + _spline[1]) * p + _spline[2];
                                //dRho = (spline[m][0]*p + spline[m][1])*p + spline[m][2];

                                psip = (_df[kk] + _df[n]) * dRho;
                                fpair = -psip * recip;
                                //printf("pair:%d, %lf, %lf, %lf, %lf, %lf, %lf, %lf, %.15lf\n", m, _spline[2], z2, z2p, _df[kk], _x[n * 3], _x[n * 3+1], _x[n * 3+2], fpair);

                                _f[kk * 3] += delx * fpair;
                                _f[kk * 3 + 1] += dely * fpair;
                                _f[kk * 3 + 2] += delz * fpair;

                                _f[n * 3] -= delx * fpair;
                                _f[n * 3 + 1] -= dely * fpair;
                                _f[n * 3 + 2] -= delz * fpair;
                                //if(my_id == 0)
                                //	printf("tmp:%lf, %lf, %lf, %.18lf\n", z2, z2p, psip, fpair);
                                //if(kk==(nghostx*2+6+3))
                                //	printf("kk+:%.15lf\n", _f[kk*3]);
                                //if(n==(nghostx*2+6+3))
                                //       printf("n-:%.15lf\n", _f[n*3]);
                            }
                        }
                    }
                    //if(my_id == 0)
                    //	printf("f:%.15lf\n", _f_put[kk*3]);
                    kk++;
                }
                put_reply = 0;
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k);
                athread_put(PE_MODE, &_f[0], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8, 16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 1);
                athread_put(PE_MODE, &_f[16 * 15], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                kk = IndexOf3DIndex(8 * (m_step % n_block), j - 2, k + 2);
                athread_put(PE_MODE, &_f[16 * 10 * 3], &f[kk * 3], 16 * 15 * 8, &put_reply, (nghostx - 16) * 3 * 8,
                            16 * 3 * 8);
                while (put_reply != 3);
            }
        }
    }

    ldm_free(_x, sizeof(double) * (16 * 15 * 3));
    ldm_free(_f, sizeof(double) * (16 * 15 * 3));
    ldm_free(_df, sizeof(double) * (16 * 15));
    ldm_free(_spline, sizeof(double) * 7);
}

int BeginOfArea(int natom, int start, int subindex) {
    return start + ((natom / Nvalue) * subindex);
}

int EndOfArea(int natom, int start, int subindex) {
    return start + ((natom / Nvalue) * (subindex + 1));
}

long int IndexOf3DIndex(long int xIndex, long int yIndex, long int zIndex) {
    return (zIndex * nghosty + yIndex) * nghostx + xIndex;
}

void rtc(unsigned long *counter) {
    unsigned long rpcc;
    asm volatile("rcsr %0,4":"=r"(rpcc));
    *counter = rpcc;
}
