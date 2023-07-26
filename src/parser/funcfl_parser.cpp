//
// Created by genshen on 2018-12-23.
//

#include "funcfl_parser.h"
#include <utils.h>

FuncflParser::FuncflParser(std::istream &pot_file) : Parser(pot_file) {}

void FuncflParser::parseHeader() {
  // the 1st line of the file
  char tmp[4096];
  pot_file.getline(tmp, sizeof(tmp));
  char name[3];
  sscanf(tmp, "%s", name);

  // the 2nd line of the file
  atom_type::_type_atomic_no nAtomic;
  double mass, lat;
  char latticeType[8];
  pot_file.getline(tmp, sizeof(tmp));
  sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);

  // todo eam_instance->setlatticeType(latticeType); // lattice type

  // the 3rd line of the file
  pot_file.getline(tmp, sizeof(tmp));
  sscanf(tmp, "%d %le %d %le %le", &nRho, &dRho, &nR, &dR, &cutoff);
  type_lists.addAtomProp(nAtomic, "", mass, lat, cutoff);
}

void FuncflParser::parseBody(eam *eam_instance) {
  double x0 = 0.0;
  int bufSize = std::max(nRho, nR);
  double *buf = new double[bufSize];

  // read embedded energy table
  for (int ii = 0; ii < nRho; ++ii) {
    pot_file >> buf[ii];
  }
  //   fixme eam_instance->initf(0, nRho, x0, dRho, buf); //通过读取势文件的数据建立table

  // read pair potnetial table
  for (int ii = 0; ii < nR; ++ii) {
    pot_file >> buf[ii];
  }
  double r;
  for (int ii = 1; ii < nR; ++ii) {
    r = x0 + ii * dR;
    buf[ii] *= buf[ii] / r;
    buf[ii] *= hartreeToEv * bohrToAngs;
  }
  buf[0] = buf[1] + (buf[1] - buf[2]);
  // fixme  eam_instance->initphi(0, nR, x0, dR, buf);

  // read electron density table
  for (int ii = 0; ii < nR; ++ii) {
    pot_file >> buf[ii];
  }
  //  fixme  eam_instance->initrho(0, nR, x0, dR, buf);

  delete[] buf;
}