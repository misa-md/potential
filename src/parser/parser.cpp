//
// Created by genshen on 2018-12-23.
//

#include "parser.h"
#include <cstdlib>
#include <cstring>
#include <string>

Parser::Parser(const std::string filename)
    : file_ele_size(0), filter_ele_size(0), filter_ele_types(), pot_filename(filename) {
  char tmp[4096];
  sprintf(tmp, "%s", pot_filename.c_str());

  pot_file = fopen(tmp, "r");
  if (pot_file == nullptr) {
    printf("error, open file %s failed.\n", pot_filename.c_str());
    return; // todo error
  }
}

void Parser::setFilterEleTypes(const std::vector<atom_type::_type_prop_key> ele_types) {
  filter_ele_types = ele_types;
  if (!filter_ele_types.empty()) {
    filter_ele_size = filter_ele_types.size();
  }
}

bool Parser::isInFilterList(atom_type::_type_prop_key key) {
  for (atom_type::_type_prop_key k : filter_ele_types) {
    if (k == key) {
      return true;
    }
  }
  return false;
}

void Parser::done() { fclose(pot_file); }

void Parser::grab(FILE *fptr, int n, double *list) {
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

atom_type::_type_atom_types Parser::getEles() const {
  if (isEleTypesFilterEnabled()) {
    return filter_ele_size;
  }
  return file_ele_size;
}
