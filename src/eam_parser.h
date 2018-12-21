//
// Created by baihe back to 2015-12-26.
//

#ifndef CRYSTAL_MD_EAM_PARSER_H
#define CRYSTAL_MD_EAM_PARSER_H

#include <string>

#include "eam.h"

/// 1 amu in kilograms
#define amuInKilograms  1.660538921e-27

/// 1 fs in seconds
#define fsInSeconds     1.0e-15

/// 1 Ang in meters
#define AngsInMeters    1.0e-10

/// 1 eV in Joules
#define eVInJoules      1.602176565e-19

/// Internal mass units are eV * fs^2 / Ang^2
static const double amuToInternalMass =
        amuInKilograms * AngsInMeters * AngsInMeters
        / (fsInSeconds * fsInSeconds * eVInJoules);

/// Hartrees to eVs
static const double hartreeToEv = 27.21138505;

/// Bohrs to Angstroms
static const double bohrToAngs = 0.52917721092;

/**
 * parse eam potential from input file specified by {@var potential_filename}
 */

class EamParser {
public:
    /**
     *
     * @param potential_filename file path of potential file.
     * @param file_type the file type of potential file. There are 2 types (formats) of potential file, namely "funcfl" and "setfl".
     */
    EamParser(const std::string &potential_filename, const std::string &file_type);

//    EamParser();

    /**
     * start parse, the parse result will be stored in {@var eam}.
     * @return true for parsing success, false for otherwise.
     */
    bool parse(eam *eam_instance);

private:
    const std::string potential_filename; // path of eam potential file.
    const std::string file_type; // eam potential file type, "funcfl" or "setfl".

    void grab(FILE *fptr, int n, double *list); // called in parsing.

    void parseEamSetfl(eam *eam_instance, FILE *potFile);

    void parseEamFuncfl(eam *eam_instance, FILE *potFile);
};

#endif //CRYSTAL_MD_EAM_PARSER_H
