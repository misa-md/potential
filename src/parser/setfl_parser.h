//
// Created by baihe back to 2015-12-26.
// updated by genshen on 2018-12-23.
//

#ifndef POT_SETFL_PARSER_H
#define POT_SETFL_PARSER_H

#include <iostream>

#include "eam_base_parser.hpp"
#include "parser.h"

struct EamPotFileHeader {
  int nRho, nR;            // at line 5 in potential file header.
  double dRho, dR, cutoff; // at line 5 in potential file header
};

// parser for parsing potential file of "setdl" format.
class SetflParser : public EamBaseParse {
public:
  explicit SetflParser(std::istream &pot_file);

  void parseHeader() override;

  void parseBody(eam *eam_instance) override;

  inline EamPotFileHeader getHeader() const { return header; }

protected:
  // parse eam file of "eam/alloy" style.
  void parseBodyEamAlloy(EamAlloyLoader *pot_loader);

  // parse eam file of "eam/fs" style.
  void parseBodyEamFs(EamFsLoader *pot_loader);

private:
  EamPotFileHeader header;
};

#endif // POT_SETFL_PARSER_H
