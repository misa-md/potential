#include <mpi.h>
#include <cstring>
#include <cstdio>
#include <logs/logs.h>

#include "eam.h"

eam::eam() {};

eam::~eam() {
    delete[] f;
    delete[] phi;
    delete[] rho;
//	delete[] mass;
};

void eam::setatomicNo(double nAtomic) {
    atomicNo = nAtomic;
}

void eam::setlat(double latticeconst) {
    lat = latticeconst;
}

void eam::setmass(int i, double _mass) {
    mass[i] = _mass;
}

void eam::setlatticeType(char *_latticeType) {
    strcpy(latticeType, _latticeType);
}

void eam::setname(char *_name) {
    strcpy(name, _name);
}

void eam::setcutoff(double _cutoff) {
    cutoff = _cutoff;
}

void eam::init(int nElems) {
    _nElems = nElems;
    f = new InterpolationObject[nElems];
    phi = new InterpolationObject[nElems + nElems * (nElems - 1) / 2]; // self to self plus self to others
    rho = new InterpolationObject[nElems];
    mass = new double[nElems];
}

void eam::initf(int i, int nRho, double x0, double dRho, double *buf) {
    f[i].initInterpolationObject(nRho, x0, dRho, buf);
}

void eam::initphi(int i, int nR, double x0, double dR, double *buf) {
    phi[i].initInterpolationObject(nR, x0, dR, buf);
}

void eam::initrho(int i, int nR, double x0, double dR, double *buf) {
    rho[i].initInterpolationObject(nR, x0, dR, buf);
}

void eam::eamBCast(int rank) {
    MPI_Bcast(&_nElems, 1, MPI_INT, MASTER_PROCESSOR, MPI_COMM_WORLD);
    if (rank != MASTER_PROCESSOR) {
        this->init(_nElems); // initialize array for storing eam data.
    }
    MPI_Bcast(mass, _nElems, MPI_DOUBLE, MASTER_PROCESSOR, MPI_COMM_WORLD);

    for (int i = 0; i < _nElems; i++) {
        rho[i].bcastInterpolationObject(rank);
        f[i].bcastInterpolationObject(rank);
    }
    for (int i = 0; i < (_nElems + _nElems * (_nElems - 1) / 2); i++) {
        phi[i].bcastInterpolationObject(rank);
    }
}

void eam::interpolateFile() {
    for (int i = 0; i < _nElems; i++) {
        rho[i].interpolatefile();
        f[i].interpolatefile();
    }
    for (int i = 0; i < (_nElems + _nElems * (_nElems - 1) / 2); i++) {
        phi[i].interpolatefile();
    }
}

// todo rewrite.
bool eam::parse(const std::string &potential_filename, const std::string &file_type) {
    if (file_type == "funcfl") {
        char tmp[4096];
        sprintf(tmp, "%s", potential_filename.c_str());
        FILE *potFile = fopen(tmp, "r");
        if (potFile == nullptr) {
            kiwi::logs::e("eam", "file {} not found.\n", potential_filename);
            return false;
        }

        // 第一行
        fgets(tmp, sizeof(tmp), potFile);
        char name[3];
        sscanf(tmp, "%s", name);
        setname(name);

        // 第二行
        int nAtomic;
        double mass, lat;
        char latticeType[8];
        fgets(tmp, sizeof(tmp), potFile);
        sscanf(tmp, "%d %le %le %s", &nAtomic, &mass, &lat, latticeType);

        setatomicNo(nAtomic); // 原子序号
        setlat(lat); // 晶格常数
        setmass(0, mass); // 质量.
        setlatticeType(latticeType); //晶格类型

        // 第三行
        int nRho, nR;
        double dRho, dR, cutoff;
        fgets(tmp, sizeof(tmp), potFile);
        sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
        setcutoff(cutoff); //截断半径
        double x0 = 0.0;

        // 申请读取数据的空间
        int bufSize = std::max(nRho, nR);
        double *buf = new double[bufSize];

        // 读取嵌入能表
        for (int ii = 0; ii < nRho; ++ii) {
            fscanf(potFile, "%lg", buf + ii);
        }
        initf(0, nRho, x0, dRho, buf); //通过读取势文件的数据建立table

        // 读取对势表
        for (int ii = 0; ii < nR; ++ii) {
            fscanf(potFile, "%lg", buf + ii);
        }
        double r;
        for (int ii = 1; ii < nR; ++ii) {
            r = x0 + ii * dR;
            buf[ii] *= buf[ii] / r;
            buf[ii] *= hartreeToEv * bohrToAngs;
        }
        buf[0] = buf[1] + (buf[1] - buf[2]);
        initphi(0, nR, x0, dR, buf);

        // 读取电子云密度表
        for (int ii = 0; ii < nR; ++ii) {
            fscanf(potFile, "%lg", buf + ii);
        }
        initrho(0, nR, x0, dR, buf);

        delete[] buf;
    } else if (file_type == std::string("setfl")) {
        char tmp[4096];
        sprintf(tmp, "%s", potential_filename.c_str());

        FILE *potFile = fopen(tmp, "r");
        if (potFile == nullptr) {
            kiwi::logs::e("eam", "file {} not found.\n", potential_filename);
            return false;
        }

        // 前三行为注释
        fgets(tmp, sizeof(tmp), potFile);
        fgets(tmp, sizeof(tmp), potFile);
        fgets(tmp, sizeof(tmp), potFile);

        // 第四行
        fgets(tmp, sizeof(tmp), potFile);
        int nElemTypes;
        sscanf(tmp, "%d", &nElemTypes); //原子类型个数

        init(nElemTypes);// 从文件中读入原子类型个数后, 对势函数进行初始化.

        char *copy;
        copy = new char[strlen(tmp) + 1];
        strcpy(copy, tmp);
        char *ptr;
        if ((ptr = strchr(copy, '#'))) *ptr = '\0';
        int n;
        if (strtok(copy, " \t\n\r\f") == nullptr) {
            n = 0;
        } else {
            n = 1;
            while (strtok(nullptr, " \t\n\r\f")) n++;
        }
        int nwords = n;
        delete[] copy;
        if (nwords != nElemTypes + 1) {
            kiwi::logs::e("eam", "Incorrect element names in EAM potential file!");
            // todo MPI abort.
        }

        char **words = new char *[nElemTypes + 1];
        nwords = 0;
        strtok(tmp, " \t\n\r\f");
        while ((words[nwords++] = strtok(nullptr, " \t\n\r\f"))) {
            continue;
        }
        delete[] words;
        // 第五行
        int nRho, nR;
        double dRho, dR, cutoff;
        // 所有原子使用同一个截断半径
        fgets(tmp, sizeof(tmp), potFile);
        sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
        setcutoff(cutoff);

        // 申请读取数据空间
        int bufSize = std::max(nRho, nR);
        double *buf = new double[bufSize];
        double x0 = 0.0;
        // 每种原子信息
        for (int i = 0; i < nElemTypes; i++) {
            fgets(tmp, sizeof(tmp), potFile);
            int nAtomic;
            double mass, lat;
            char latticeType[8];
            sscanf(tmp, "%d %le %le %s", &nAtomic, &mass, &lat, latticeType);

            setmass(i, mass);  // 原子质量

            // 读取嵌入能表
            grab(potFile, nRho, buf);
            initf(i, nRho, x0, dRho, buf);

            // 读取电子云密度表
            grab(potFile, nR, buf);
            initrho(i, nR, x0, dR, buf);
        }

        //读取对势表
        int i, j, k = 0;
        for (i = 0; i < nElemTypes; i++) {
            for (j = 0; j <= i; j++) {
                grab(potFile, nR, buf);
                initphi(k++, nR, x0, dR, buf);
            }
        }
        delete[] buf;
    }
    return true;
}

void eam::grab(FILE *fptr, int n, double *list) {
    char *ptr;
    char line[1024];

    int i = 0;
    while (i < n) {
        fgets(line, 1024, fptr);
        ptr = strtok(line, " \t\n\r\f");
        list[i++] = atof(ptr);
        while ((ptr = strtok(nullptr, " \t\n\r\f"))) {
            list[i++] = atof(ptr);
        }
    }
}