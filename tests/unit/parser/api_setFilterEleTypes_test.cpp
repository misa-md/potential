//
// Created by genshen on 2023/7/24.
//

#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "../eam_pot_fixture.hpp"
#include "parser/setfl_parser.h"

void dump_to_buffer(const int eam_style, const int eles_num, EamPotFileHeader header, eam *_pot, std::ostream &out);

#define DO_TABLE_COMPARE(STD_API, TEST_API)                                                                            \
  {                                                                                                                    \
    auto tab = STD_API;                                                                                                \
    auto test_tab = TEST_API;                                                                                          \
    for (int k = 0; k < tab->n; k++) {                                                                                 \
      EXPECT_EQ(test_tab->values[k], tab->values[k]);                                                                  \
    }                                                                                                                  \
  }

TEST_F(EamPotAlloyTestFixture, test_eam_alloy_parser_setFilterEleTypes) {
  std::stringstream s;
  //  std::ofstream s("out.tmp.txt"); // can also dump to file.
  EamPotFileHeader header = {.nRho = DATA_SIZE, .nR = DATA_SIZE, .dRho = DELTA, .dR = DELTA, .cutoff = 0.5};
  dump_to_buffer(EAM_STYLE_ALLOY, ELE_SIZE, header, _pot, s);

  // new parser
  SetflParser *parser = new SetflParser(s);
  parser->setFilterEleTypes({0, 2});

  // parse header
  parser->parseHeader();
  EXPECT_EQ(parser->getEles(), 2);
  parser->getHeader();

  // parse body
  auto test_pot = eam::newInstance(EAM_STYLE_ALLOY, parser->getEles(), 0, 0, MPI_COMM_WORLD);
  parser->parseBody(test_pot);

  // do assert
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEmbedded(0), test_pot->eam_pot_loader->loadEmbedded(0));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEmbedded(2), test_pot->eam_pot_loader->loadEmbedded(1));

  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadElectronDensity(0), test_pot->eam_pot_loader->loadElectronDensity(0));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadElectronDensity(2), test_pot->eam_pot_loader->loadElectronDensity(1));

  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(0, 0), test_pot->eam_pot_loader->loadEamPhi(0, 0));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(0, 2), test_pot->eam_pot_loader->loadEamPhi(0, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(2, 2), test_pot->eam_pot_loader->loadEamPhi(1, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(2, 0), test_pot->eam_pot_loader->loadEamPhi(1, 0));
}

TEST_F(EamPotFsTestFixture, test_eam_fs_parser_setFilterEleTypes) {
  std::stringstream s;
  //  std::ofstream s("out.tmp.txt"); // can also dump to file.
  EamPotFileHeader header = {.nRho = DATA_SIZE, .nR = DATA_SIZE, .dRho = DELTA, .dR = DELTA, .cutoff = 0.5};
  dump_to_buffer(EAM_STYLE_FS, ELE_SIZE, header, _pot, s);

  // new parser
  SetflParser *parser = new SetflParser(s);
  parser->setFilterEleTypes({1, 2});

  // parse header
  parser->parseHeader();
  EXPECT_EQ(parser->getEles(), 2);
  parser->getHeader();

  // parse body
  auto test_pot = eam::newInstance(EAM_STYLE_FS, parser->getEles(), 0, 0, MPI_COMM_WORLD);
  parser->parseBody(test_pot);

  // do assert
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEmbedded(1), test_pot->eam_pot_loader->loadEmbedded(0));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEmbedded(2), test_pot->eam_pot_loader->loadEmbedded(1));

  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadElectronDensity(1, 2),
                   test_pot->eam_pot_loader->loadElectronDensity(0, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadElectronDensity(2, 2),
                   test_pot->eam_pot_loader->loadElectronDensity(1, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadElectronDensity(2, 1),
                   test_pot->eam_pot_loader->loadElectronDensity(1, 0));

  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(1, 1), test_pot->eam_pot_loader->loadEamPhi(0, 0));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(1, 2), test_pot->eam_pot_loader->loadEamPhi(0, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(2, 2), test_pot->eam_pot_loader->loadEamPhi(1, 1));
  DO_TABLE_COMPARE(_pot->eam_pot_loader->loadEamPhi(2, 1), test_pot->eam_pot_loader->loadEamPhi(1, 0));
}
