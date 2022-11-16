//
// Created by genshen on 2022/9/22.
//

#ifndef POT_EAM_BASE_PARSER_HPP
#define POT_EAM_BASE_PARSER_HPP

#include <stdlib.h>

#include "parser.h"

class EamBaseParse : public Parser {

public:
  explicit EamBaseParse(const std::string &filename) : Parser(filename) {}

protected:
  template <typename T> static void grab(FILE *fptr, int n, T *list) {
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
};

#endif // POT_EAM_BASE_PARSER_HPP
