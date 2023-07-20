//
// origin code created by baihe back to 2015-12-26.
// updated by genshen on 2018-12-23.
//

#ifndef POT_FUNL_PARSER_H
#define POT_FUNL_PARSER_H

#include "parser.h"

/**
 * @deprecated FuncflParser is not maintained any more.
 */
class FuncflParser : public Parser {
public:
  explicit FuncflParser(std::istream &pot_file);

  void parseHeader() override;

  void parseBody(eam *eam_instance) override;

private:
  int nRho, nR;            // at line 3 in header.
  double dRho, dR, cutoff; // at line 3 in header
};

#endif // POT_FUNL_PARSER_H
