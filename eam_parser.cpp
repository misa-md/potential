#include <mpi.h>
#include <cstring>
#include <cstdio>
#include <logs/logs.h>

#include "eam_parser.h"

EamParser::EamParser(const std::string &potential_filename, const std::string &file_type)
        : potential_filename(potential_filename), file_type(file_type) {
}

// todo rewrite.
bool EamParser::parse(eam *eam_instance) {
    if (file_type == "funcfl") {
        char tmp[4096];
        sprintf(tmp, "%s", potential_filename.c_str());
        FILE *potFile = fopen(tmp, "r");
        if (potFile == nullptr) {
            kiwi::logs::e("eam", "open file {} failed.\n", potential_filename);
            return false;
        }
        parseEamFuncfl(eam_instance, potFile);
    } else if (file_type == "setfl") {
        char tmp[4096];
        sprintf(tmp, "%s", potential_filename.c_str());

        FILE *potFile = fopen(tmp, "r");
        if (potFile == nullptr) {
            kiwi::logs::e("eam", "open file {} failed.\n", potential_filename);
            return false;
        }
        parseEamSetfl(eam_instance, potFile);
    }
    return true;
}

void EamParser::parseEamFuncfl(eam *eam_instance, FILE *potFile) {
    // 第一行
    char tmp[4096];
    fgets(tmp, sizeof(tmp), potFile);
    char name[3];
    sscanf(tmp, "%s", name);
    eam_instance->setname(name);

    // 第二行
    int nAtomic;
    double mass, lat;
    char latticeType[8];
    fgets(tmp, sizeof(tmp), potFile);
    sscanf(tmp, "%d %le %le %s", &nAtomic, &mass, &lat, latticeType);

    eam_instance->setatomicNo(nAtomic); // 原子序号
    eam_instance->setlat(lat); // 晶格常数
    eam_instance->setmass(0, mass); // 质量.
    eam_instance->setlatticeType(latticeType); //晶格类型

    // 第三行
    int nRho, nR;
    double dRho, dR, cutoff;
    fgets(tmp, sizeof(tmp), potFile);
    sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
    eam_instance->setcutoff(cutoff); //截断半径
    double x0 = 0.0;

    // 申请读取数据的空间
    int bufSize = std::max(nRho, nR);
    double *buf = new double[bufSize];

    // 读取嵌入能表
    for (int ii = 0; ii < nRho; ++ii) {
        fscanf(potFile, "%lg", buf + ii);
    }
    eam_instance->initf(0, nRho, x0, dRho, buf); //通过读取势文件的数据建立table

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
    eam_instance->initphi(0, nR, x0, dR, buf);

    // 读取电子云密度表
    for (int ii = 0; ii < nR; ++ii) {
        fscanf(potFile, "%lg", buf + ii);
    }
    eam_instance->initrho(0, nR, x0, dR, buf);

    delete[] buf;
}


void EamParser::parseEamSetfl(eam *eam_instance, FILE *potFile) {
    char tmp[4096];
    // 前三行为注释
    fgets(tmp, sizeof(tmp), potFile);
    fgets(tmp, sizeof(tmp), potFile);
    fgets(tmp, sizeof(tmp), potFile);

    // 第四行
    fgets(tmp, sizeof(tmp), potFile);
    int nElemTypes;
    sscanf(tmp, "%d", &nElemTypes); //原子类型个数

    eam_instance->init(nElemTypes);// 从文件中读入原子类型个数后, 对势函数进行初始化.

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
    eam_instance->setcutoff(cutoff);

    // 申请读取数据空间
    int bufSize = std::max(nRho, nR);
    double *buf = new double[bufSize];
    double x0 = 0.0; // fixme start from 0 ??
    // 每种原子信息
    for (int i = 0; i < nElemTypes; i++) {
        fgets(tmp, sizeof(tmp), potFile);
        int nAtomic;
        double mass, lat;
        char latticeType[8];
        sscanf(tmp, "%d %le %le %s", &nAtomic, &mass, &lat, latticeType);

        eam_instance->setmass(i, mass);  // 原子质量

        // 读取嵌入能表
        grab(potFile, nRho, buf);
        eam_instance->initf(i, nRho, x0, dRho, buf);

        // 读取电子云密度表
        grab(potFile, nR, buf);
        eam_instance->initrho(i, nR, x0, dR, buf);
    }

    //读取对势表
    int i, j, k = 0;
    for (i = 0; i < nElemTypes; i++) {
        for (j = 0; j <= i; j++) {
            grab(potFile, nR, buf);
            eam_instance->initphi(k++, nR, x0, dR, buf);
        }
    }
    delete[] buf;
}

void EamParser::grab(FILE *fptr, int n, double *list) {
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