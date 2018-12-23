//
// Created by genshen on 2018-12-23.
//

#include <cstring>
#include <cstdlib>
#include "utils.h"
#include "eam.h"
#include "setfl_parser.h"

SetflParser::SetflParser(const std::string filename) : Parser(filename) {}

void SetflParser::parseHeader() {
    char tmp[4096];
    // 前三行为注释
    fgets(tmp, sizeof(tmp), pot_file);
    fgets(tmp, sizeof(tmp), pot_file);
    fgets(tmp, sizeof(tmp), pot_file);

    // 第四行
    fgets(tmp, sizeof(tmp), pot_file);
//    int nElemTypees;
    sscanf(tmp, "%hu", &elements_size); // 原子类型个数

//    eam_instance->initElementN(nElemTypes);// 从文件中读入原子类型个数后, 对势函数进行初始化.
// todo delete.

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
    if (nwords != elements_size + 1) {
        printf("Incorrect element names in EAM potential file!");
        // todo MPI abort.
    }

    char **words = new char *[elements_size + 1];
    nwords = 0;
    strtok(tmp, " \t\n\r\f");
    while ((words[nwords++] = strtok(nullptr, " \t\n\r\f"))) {
        continue;
    }
    delete[] words;
    // 第五行
    // 所有原子使用同一个截断半径
    fgets(tmp, sizeof(tmp), pot_file);
    sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
}

void SetflParser::parseBody(eam *eam_instance) {
    // 申请读取数据空间
    char tmp[4096];
    int bufSize = std::max(nRho, nR);
    double *buf = new double[bufSize];
    double x0 = 0.0; // fixme start from 0 ??
    atom_type::_type_prop_key *prop_key_list = new atom_type::_type_prop_key[elements_size];
    // 每种原子信息
    for (int i = 0; i < elements_size; i++) {
        fgets(tmp, sizeof(tmp), pot_file);
        atom_type::_type_atomic_no nAtomic;
        double mass, lat; // mass, lattice const
        char latticeType[8]; // lattice type.
        sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);
        atom_type::_type_prop_key key = type_lists.addAtomProp(nAtomic, "", mass, lat,
                                                                             cutoff); // todo ele name
        prop_key_list[i] = key;

        // 读取嵌入能表
        grab(pot_file, nRho, buf);
        eam_instance->embedded.append(key, nRho, x0, dRho, buf);

        // 读取电子云密度表
        grab(pot_file, nR, buf);
        eam_instance->electron_density.append(key, nR, x0, dR, buf);
    }

    //读取对势表
    int i, j;
    for (i = 0; i < elements_size; i++) {
        for (j = 0; j <= i; j++) {
            grab(pot_file, nR, buf);
            eam_instance->eam_phi.append(prop_key_list[i], prop_key_list[j], nR, x0, dR, buf);
        }
    }
    delete[] buf;
    delete[] prop_key_list;
}
