//
// Created by baihe back to 2015-12-26.
// updated by genshen on 2018-12-23.
//

#ifndef POT_SETFL_PARSER_H
#define POT_SETFL_PARSER_H

#include "parser.h"

// parser for parsing potential file of "setdl" format.
class SetflParser : public Parser {
public:
  explicit SetflParser(const std::string filename);

  void parseHeader() override;

  void parseBody(eam *eam_instance) override;

protected:
  // parse eam file of "eam/alloy" style.
  void parseBodyEamAlloy(EamAlloyLoader *pot_loader);

  // parse eam file of "eam/fs" style.
  void parseBodyEamFs(EamFsLoader *pot_loader);

private:
  int nRho, nR;            // at line 5 in header.
  double dRho, dR, cutoff; // at line 5 in header
};

#endif // POT_SETFL_PARSER_H
