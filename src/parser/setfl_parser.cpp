//
// Created by genshen on 2018-12-23.
//

#include "setfl_parser.h"
#include "eam.h"
#include "utils.h"
#include <cstdlib>
#include <cstring>

SetflParser::SetflParser(std::istream &pot_file) : EamBaseParse(pot_file) {}

void SetflParser::parseHeader() {
  char tmp[4096];
  // file comments in the first 3 lines of the file.
  std::string str_tmp;
  std::getline(pot_file, str_tmp);
  std::getline(pot_file, str_tmp);
  std::getline(pot_file, str_tmp);

  // line 4 in file
  pot_file.getline(&tmp[0], sizeof(tmp));
  sscanf(tmp, "%hu", &file_ele_size); // number of atom types


  char *copy;
  copy = new char[strlen(tmp) + 1];
  strcpy(copy, tmp);
  char *ptr;
  if ((ptr = strchr(copy, '#'))) {
    *ptr = '\0';
  }
  int n;
  if (strtok(copy, " \t\n\r\f") == nullptr) {
    n = 0;
  } else {
    n = 1;
    while (strtok(nullptr, " \t\n\r\f")) {
      n++;
    }
  }
  int nwords = n;
  delete[] copy;
  if (nwords != file_ele_size + 1) {
    printf("Incorrect element names in EAM potential file!");
    // todo MPI abort.
  }

  char **words = new char *[file_ele_size + 1];
  nwords = 0;
  strtok(tmp, " \t\n\r\f");
  while ((words[nwords++] = strtok(nullptr, " \t\n\r\f"))) {
    continue;
  }
  delete[] words;
  // line 5 in file
  // all types of atom use the same cutoff.
  pot_file.getline(&tmp[0], sizeof(tmp));
  sscanf(tmp, "%d %le %d %le %le", &header.nRho, &header.dRho, &header.nR, &header.dR, &header.cutoff);
}

void SetflParser::parseBody(eam *eam_instance) {
  if (eam_instance->eam_style == EAM_STYLE_ALLOY) {
    // parse eam potential file of style "eam/alloy"
    parseBodyEamAlloy(dynamic_cast<EamAlloyLoader *>(eam_instance->eam_pot_loader));
  } else if (eam_instance->eam_style == EAM_STYLE_FS) {
    // parse eam potential file of style "eam/fs"
    parseBodyEamFs(dynamic_cast<EamFsLoader *>(eam_instance->eam_pot_loader));
  }
}

void SetflParser::parseBodyEamAlloy(EamAlloyLoader *pot_loader) {
  char tmp[4096];
  const int bufSize = std::max(header.nRho, header.nR);
  double *buf = new double[bufSize];
  double x0 = 0.0; // fixme start from 0 ??
  atom_type::_type_prop_key *prop_key_list = new atom_type::_type_prop_key[file_ele_size];

  // for each type of atom
  for (int i = 0; i < file_ele_size; i++) {
    pot_file.getline(&tmp[0], sizeof(tmp));
    atom_type::_type_atomic_no nAtomic;
    double mass, lat;    // mass, lattice const
    char latticeType[8]; // lattice type.
    sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);

    atom_type::_type_prop_key key = AtomPropsList::makeId(i);
    prop_key_list[i] = key;
    if (!isEleTypesFilterEnabled() || isInFilterList(key)) {
      type_lists.addAtomProp(nAtomic, "", mass, lat, header.cutoff); // todo ele name
    }

    // read embedded energy table
    grab(pot_file, header.nRho, buf);
    if (!isEleTypesFilterEnabled() || isInFilterList(key)) {
      pot_loader->embedded.append(key, header.nRho, x0, header.dRho, buf);
    }

    // read electron density table
    grab(pot_file, header.nR, buf);
    if (!isEleTypesFilterEnabled() || isInFilterList(key)) {
      pot_loader->electron_density.append(key, header.nR, x0, header.dR, buf);
    }
  }

  // read pair potential table
  int i, j;
  for (i = 0; i < file_ele_size; i++) {
    for (j = 0; j <= i; j++) {
      grab(pot_file, header.nR, buf);
      if (!isEleTypesFilterEnabled() || (isInFilterList(prop_key_list[i]) && isInFilterList(prop_key_list[j]))) {
        pot_loader->eam_phi.append(prop_key_list[i], prop_key_list[j], header.nR, x0, header.dR, buf);
      }
    }
  }
  delete[] buf;
  delete[] prop_key_list;
}

void SetflParser::parseBodyEamFs(EamFsLoader *pot_loader) {
  char tmp[4096];
  const int bufSize = std::max(header.nRho, header.nR);
  double *buf = new double[bufSize];
  double x0 = 0.0; // fixme start from 0 ??
  atom_type::_type_prop_key *prop_key_list = new atom_type::_type_prop_key[file_ele_size];

  for (int i = 0; i < file_ele_size; i++) {
    pot_file.getline(&tmp[0], sizeof(tmp));
    atom_type::_type_atomic_no nAtomic;
    double mass, lat;    // mass, lattice const
    char latticeType[8]; // lattice type.
    sscanf(tmp, "%hu %le %le %s", &nAtomic, &mass, &lat, latticeType);

    atom_type::_type_prop_key key = AtomPropsList::makeId(i);
    prop_key_list[i] = key;
    if (!isEleTypesFilterEnabled() || isInFilterList(key)) {
      type_lists.addAtomProp(nAtomic, "", mass, lat, header.cutoff); // todo ele name
    }
    // read embedded energy table
    grab(pot_file, header.nRho, buf);
    if (!isEleTypesFilterEnabled() || isInFilterList(key)) {
      pot_loader->embedded.push_back(header.nRho, x0, header.dRho, buf);
    }
    // read electron charge density
    for (int j = 0; j < file_ele_size; j++) {
      grab(pot_file, header.nR, buf);
      atom_type::_type_prop_key key2 = j;
      if (!isEleTypesFilterEnabled() || (isInFilterList(key) && isInFilterList(key))) {
        pot_loader->electron_density.push_back(header.nR, x0, header.dR, buf);
      }
    }
  }

  // read pair potentials
  int i, j;
  for (i = 0; i < file_ele_size; i++) {
    for (j = 0; j <= i; j++) {
      grab(pot_file, header.nR, buf);
      if (!isEleTypesFilterEnabled() || (isInFilterList(prop_key_list[i]) && isInFilterList(prop_key_list[j]))) {
        pot_loader->eam_phi.push_back(header.nR, x0, header.dR, buf);
      }
    }
  }
  delete[] buf;
}
