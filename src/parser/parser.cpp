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
  snprintf(tmp, pot_filename.size() + 1, "%s", pot_filename.c_str());

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

atom_type::_type_atom_types Parser::getEles() const {
  if (isEleTypesFilterEnabled()) {
    return filter_ele_size;
  }
  return file_ele_size;
}
