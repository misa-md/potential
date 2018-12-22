#include <mpi.h>
#include <cstring>
#include <cstdlib>

#include "eam_parser.h"
#include "utils.h"

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
            printf("error, open file %s failed.\n", potential_filename.c_str());
            return false;
        }
        parseEamFuncfl(eam_instance, potFile); // fixme return.
    } else if (file_type == "setfl") {
        char tmp[4096];
        sprintf(tmp, "%s", potential_filename.c_str());

        FILE *potFile = fopen(tmp, "r");
        if (potFile == nullptr) {
            printf("error, open file %s failed.\n", potential_filename.c_str());
            return false;
        }
        parseEamSetfl(potFile);
    }
    return true;
}

// todo use newInstance to create eam object.
eam *EamParser::parseEamFuncfl(eam *eam_instance, FILE *potFile) {
    // 第一行
    char tmp[4096];
    fgets(tmp, sizeof(tmp), potFile);
    char name[3];
    sscanf(tmp, "%s", name);

    // 第二行
    atom_type::_type_atomic_no nAtomic;
    double mass, lat;
    char latticeType[8];
    fgets(tmp, sizeof(tmp), potFile);
    sscanf(tmp, "%d %le %le %s", &nAtomic, &mass, &lat, latticeType);

    eam_instance->setlatticeType(latticeType); //晶格类型

    // 第三行
    int nRho, nR;
    double dRho, dR, cutoff;
    fgets(tmp, sizeof(tmp), potFile);
    sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
    eam_instance->type_lists.addAtomProp(nAtomic, "", mass, lat, cutoff);
    double x0 = 0.0;

    // 申请读取数据的空间
    int bufSize = std::max(nRho, nR);
    double *buf = new double[bufSize];

    // 读取嵌入能表
    for (int ii = 0; ii < nRho; ++ii) {
        fscanf(potFile, "%lg", buf + ii);
    }
//   fixme eam_instance->initf(0, nRho, x0, dRho, buf); //通过读取势文件的数据建立table

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
// fixme  eam_instance->initphi(0, nR, x0, dR, buf);

    // 读取电子云密度表
    for (int ii = 0; ii < nR; ++ii) {
        fscanf(potFile, "%lg", buf + ii);
    }
//  fixme  eam_instance->initrho(0, nR, x0, dR, buf);

    delete[] buf;
    return eam_instance;
}


eam *EamParser::parseEamSetfl(FILE *potFile) {
    char tmp[4096];
    // 前三行为注释
    fgets(tmp, sizeof(tmp), potFile);
    fgets(tmp, sizeof(tmp), potFile);
    fgets(tmp, sizeof(tmp), potFile);

    // 第四行
    fgets(tmp, sizeof(tmp), potFile);
    int nElemTypes;
    sscanf(tmp, "%d", &nElemTypes); //原子类型个数

//    eam_instance->initElementN(nElemTypes);// 从文件中读入原子类型个数后, 对势函数进行初始化.
// todo delete.
    eam *eam_instance = eam::newInstance(nElemTypes, POT_MASTER_PROCESSOR, 0,
                                         MPI_COMM_WORLD); // fixme: set rank and comm

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
        printf("Incorrect element names in EAM potential file!");
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

    // 申请读取数据空间
    int bufSize = std::max(nRho, nR);
    double *buf = new double[bufSize];
    double x0 = 0.0; // fixme start from 0 ??
    atom_type::_type_prop_key *prop_key_list = new atom_type::_type_prop_key[nElemTypes];
    // 每种原子信息
    for (int i = 0; i < nElemTypes; i++) {
        fgets(tmp, sizeof(tmp), potFile);
        atom_type::_type_atomic_no nAtomic;
        double mass, lat; // mass, lattice const
        char latticeType[8]; // lattice type.
        sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);
        atom_type::_type_prop_key key = eam_instance->type_lists.addAtomProp(nAtomic, "", mass, lat,
                                                                             cutoff); // todo ele name
        prop_key_list[i] = key;

        // 读取嵌入能表
        grab(potFile, nRho, buf);
        eam_instance->embedded.append(key, nRho, x0, dRho, buf);

        // 读取电子云密度表
        grab(potFile, nR, buf);
        eam_instance->electron_density.append(key, nR, x0, dR, buf);
    }

    //读取对势表
    int i, j;
    for (i = 0; i < nElemTypes; i++) {
        for (j = 0; j <= i; j++) {
            grab(potFile, nR, buf);
            eam_instance->eam_phi.append(prop_key_list[i], prop_key_list[j], nR, x0, dR, buf);
        }
    }
    delete[] buf;
    delete[] prop_key_list;
    return eam_instance;
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