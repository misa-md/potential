//
// Created by genshen on 2018-12-23.
//

#include "parser.h"
#include <cstdlib>
#include <cstring>
#include <string>

Parser::Parser(std::istream &pot_file) : file_ele_size(0), filter_ele_size(0), filter_ele_types(), pot_file(pot_file) {}

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

//void Parser::done() { pot_file; }

atom_type::_type_atom_types Parser::getEles() const {
  if (isEleTypesFilterEnabled()) {
    return filter_ele_size;
  }
  return file_ele_size;
}
