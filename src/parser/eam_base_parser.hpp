//
// Created by genshen on 2022/9/22.
//

#ifndef POT_EAM_BASE_PARSER_HPP
#define POT_EAM_BASE_PARSER_HPP

#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "parser.h"

class EamBaseParse : public Parser {

public:
  explicit EamBaseParse(std::istream &pot_file) : Parser(pot_file) {}

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

  template <typename T> static void grab(std::istream &pot_file, int n, T *list) {
    char *ptr;
    char line[1024];

    int i = 0;
    while (i < n) {
      pot_file.getline(&line[0], sizeof(line));
      ptr = strtok(line, " \t\n\r\f");
      list[i++] = atof(ptr);
      while ((ptr = strtok(nullptr, " \t\n\r\f"))) {
        list[i++] = atof(ptr);
      }
    }
  }
};

#endif // POT_EAM_BASE_PARSER_HPP
