//
// Created by genshen on 2018-12-23.
//

#ifndef POT_PARSER_H
#define POT_PARSER_H

#include "container/atom_type_lists.h"
#include "eam.h"
#include "types.h"

#include <iostream>
#include <string>

/**
 * parse eam potential file.
 */
class Parser {
public:
  // all types are saved in this object while parsing.
  AtomPropsList type_lists;

  explicit Parser(std::istream &pot_file);

  /**
   * When parsing potential file, we only consider the element types in @param ele_types.
   * @note: If @param ele_types is empty, consider all element provided in potential file.
   * @param ele_types considered element types when parsing potential file.
   */
  void setFilterEleTypes(std::vector<atom_type::_type_prop_key> ele_types);

  /**
   * To check whether the element types filtering is enabled.
   * @return true for enabling element types filtering, false for otherwise.
   */
  inline bool isEleTypesFilterEnabled() const { return !(filter_ele_types.empty()); }

  /**
   * parsing header of potential file, and get necessary data such as elements count.
   */
  virtual void parseHeader() = 0;

  /**
   * parse body of potential file, and save data in potential file to eam instance
   * @param eam_instance the pointer to eam.
   */
  virtual void parseBody(eam *eam_instance) = 0;

  //  virtual void done();

  // get the real elements count.
  atom_type::_type_atom_types getEles() const;

protected:
  // the size/count of elements in potential file.
  atom_type::_type_atom_types file_ele_size;
  // the size/count of element after filtering.
  atom_type::_type_atom_types filter_ele_size;
  std::vector<atom_type::_type_prop_key> filter_ele_types;

  std::istream &pot_file;

  /**
   * check whether an element type is in the filtering list.
   * @param key id of the element type
   * @return true for existing, false for otherwise.
   */
  bool isInFilterList(const atom_type::_type_prop_key key);

private:
  const std::string pot_filename;
};

#endif // POT_PARSER_H
