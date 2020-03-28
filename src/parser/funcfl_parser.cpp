//
// Created by genshen on 2018-12-23.
//

#include "funcfl_parser.h"
#include <utils.h>

FuncflParser::FuncflParser(const std::string filename) : Parser(filename) {}

void FuncflParser::parseHeader() {
  // 第一行
  char tmp[4096];
  fgets(tmp, sizeof(tmp), pot_file);
  char name[3];
  sscanf(tmp, "%s", name);

  // 第二行
  atom_type::_type_atomic_no nAtomic;
  double mass, lat;
  char latticeType[8];
  fgets(tmp, sizeof(tmp), pot_file);
  sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);

  // todo eam_instance->setlatticeType(latticeType); //晶格类型

  // 第三行
  int nRho, nR;
  double dRho, dR, cutoff;
  fgets(tmp, sizeof(tmp), pot_file);
  sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
  type_lists.addAtomProp(nAtomic, "", mass, lat, cutoff);
}

void FuncflParser::parseBody(eam *eam_instance) {
  double x0 = 0.0;
  // 申请读取数据的空间
  int bufSize = std::max(nRho, nR);
  double *buf = new double[bufSize];

  // 读取嵌入能表
  for (int ii = 0; ii < nRho; ++ii) {
    fscanf(pot_file, "%lg", buf + ii);
  }
  //   fixme eam_instance->initf(0, nRho, x0, dRho, buf); //通过读取势文件的数据建立table

  // 读取对势表
  for (int ii = 0; ii < nR; ++ii) {
    fscanf(pot_file, "%lg", buf + ii);
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
    fscanf(pot_file, "%lg", buf + ii);
  }
  //  fixme  eam_instance->initrho(0, nR, x0, dR, buf);

  delete[] buf;
}