//
// Created by baihe back to 2015-12-26.
//

#ifndef CRYSTAL_MD_EAM_PARSER_H
#define CRYSTAL_MD_EAM_PARSER_H

#include <string>

#include "eam.h"

/**
 * parse eam potential from input file specified by {@var potential_filename}
 */
class EamParser {
public:
//    const static std::string EAM_TYPE_FUNCFL = "funcfl";
//    const static std::string EAM_TYPE_SETFL = "setfl";

    /**
     *
     * @param potential_filename file path of potential file.
     * @param file_type the file type of potential file. There are 2 types (formats) of potential file, namely "funcfl" and "setfl".
     */
    EamParser(const std::string &potential_filename, const std::string &file_type);

    /**
     * start parse, the parse result will be stored in {@var eam}.
     * @return true for parsing success, false for otherwise.
     */
    bool parse(eam *eam_instance);

private:
    const std::string potential_filename; // path of eam potential file.
    const std::string file_type; // eam potential file type, "funcfl" or "setfl".

    void grab(FILE *fptr, int n, double *list); // called in parsing.

    eam *parseEamSetfl( FILE *potFile);

    eam *parseEamFuncfl(eam *eam_instance, FILE *potFile);
};

#endif //CRYSTAL_MD_EAM_PARSER_H
