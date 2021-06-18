//
// Created by genshen on 2018-12-21.
//

#ifndef POTENTIAL_UTILS_H
#define POTENTIAL_UTILS_H

#define POT_MASTER_PROCESSOR 0

/// 1 amu in kilograms
#define amuInKilograms 1.660538921e-27

/// 1 fs in seconds
#define fsInSeconds 1.0e-15

/// 1 Ang in meters
#define AngsInMeters 1.0e-10

/// 1 eV in Joules
#define eVInJoules 1.602176565e-19

/// Internal mass units are eV * fs^2 / Ang^2
static const double amuToInternalMass =
    amuInKilograms * AngsInMeters * AngsInMeters / (fsInSeconds * fsInSeconds * eVInJoules);

/// Hartrees to eVs
static const double hartreeToEv = 27.21138505;

/// Bohrs to Angstroms
static const double bohrToAngs = 0.52917721092;

#endif // POTENTIAL_UTILS_H
