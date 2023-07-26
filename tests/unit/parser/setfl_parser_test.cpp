//
// Created by genshen on 2023/7/20.
//

#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "../eam_pot_fixture.hpp"
#include "parser/setfl_parser.h"

void dump_to_buffer(const int eam_style, const int eles_num, EamPotFileHeader header, eam *_pot, std::ostream &out) {
  // header
  out << "#comment 1" << std::endl;
  out << "#comment 2" << std::endl;
  out << "#comment 3" << std::endl;
  out << eles_num << " Fe Cu Ni" << std::endl;
  out << header.nRho << " " << header.dRho << " " << header.nR << " " << header.dR << " " << header.cutoff << std::endl;

  // body
  out.precision(17);
  for (int i = 0; i < eles_num; i++) {
    out << 26 << " " << 55.845 << " " << 2.85532 << " bcc" << std::endl;
    // dump embedded energy
    auto emb = _pot->eam_pot_loader->loadEmbedded(i);
    double *ev = emb->values;
    for (int k = 0; k < emb->n; k++) {
      out << ev[k + 1];
      if (k % 4 == 3) {
        out << std::endl;
      } else {
        out << " ";
      }
    }

    // dump electron density
    if (eam_style == EAM_STYLE_ALLOY) {
      auto density = _pot->eam_pot_loader->loadElectronDensity(i);
      double *dv = density->values;
      for (int k = 0; k < density->n; k++) {
        out << dv[k + 1];
        if (k % 4 == 3) {
          out << std::endl;
        } else {
          out << " ";
        }
      }
    } else if (eam_style == EAM_STYLE_FS) {
      for (int j = 0; j < eles_num; j++) {
        auto density = _pot->eam_pot_loader->loadElectronDensity(j, i);
        double *dv = density->values;
        for (int k = 0; k < density->n; k++) {
          out << dv[k + 1];
          if (k % 4 == 3) {
            out << std::endl;
          } else {
            out << " ";
          }
        }
      }
    }
  }
  // dump pair potential
  for (int i = 0; i < eles_num; i++) {
    for (int j = 0; j <= i; j++) {
      auto pair = _pot->eam_pot_loader->loadEamPhi(i, j);
      double *pv = pair->values;
      for (int k = 0; k < pair->n; k++) {
        out << pv[k + 1];
        if (k % 4 == 3) {
          out << std::endl;
        } else {
          out << " ";
        }
      }
    }
  }
}

// This test dumps fixture to a buffer and then parse from the buffer.
// Last, it compares the data in the parser and the fixture.
TEST_F(EamPotAlloyTestFixture, test_eam_alloy_setfl_parser) {
  std::stringstream s;
  //  std::ofstream s("out.tmp.txt"); // can also dump to file.
  EamPotFileHeader header = {.nRho = DATA_SIZE, .nR = DATA_SIZE, .dRho = DELTA, .dR = DELTA, .cutoff = 0.5};
  dump_to_buffer(EAM_STYLE_ALLOY, ELE_SIZE, header, _pot, s);

  constexpr int _ELE_SIZE = ELE_SIZE;
  // new parser
  SetflParser *parser = new SetflParser(s);
  // parse header
  parser->parseHeader();
  EXPECT_EQ(parser->getEles(), _ELE_SIZE);
  auto test_header = parser->getHeader();
  EXPECT_EQ(test_header.cutoff, header.cutoff);
  EXPECT_EQ(test_header.dR, header.dR);
  EXPECT_EQ(test_header.dRho, header.dRho);
  EXPECT_EQ(test_header.nR, header.nR);
  EXPECT_EQ(test_header.nRho, header.nRho);

  // parse body
  auto test_pot = eam::newInstance(EAM_STYLE_ALLOY, parser->getEles(), 0, 0, MPI_COMM_WORLD);
  parser->parseBody(test_pot);

  // compare body
  for (int i = 0; i < ELE_SIZE; i++) {
    // compare embedded energy
    auto emb = _pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(test_pot->eam_pot_loader, nullptr);
    auto test_emb = test_pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(test_emb, nullptr);
    EXPECT_EQ(test_emb->n, emb->n);
    EXPECT_EQ(test_emb->invDx, emb->invDx);
    // todo: x0 and max_val are generated in a different way in the fixture. we currently ignore the tests.
    // EXPECT_EQ(test_emb->x0, emb->x0);
    // EXPECT_EQ(test_emb->max_val, emb->max_val);
    for (int k = 0; k < emb->n; k++) {
      EXPECT_EQ(test_emb->values[k], emb->values[k]);
      if (test_emb->values[k] != emb->values[k]) {
        GTEST_FAIL();
      }
    }

    // compare electron density
    auto density = _pot->eam_pot_loader->loadElectronDensity(i);
    auto test_density = test_pot->eam_pot_loader->loadElectronDensity(i);
    EXPECT_EQ(test_density->n, density->n);
    EXPECT_EQ(test_density->invDx, density->invDx);
    //    EXPECT_EQ(test_density->x0, density->x0);
    //    EXPECT_EQ(test_density->max_val, density->max_val);
    for (int k = 0; k < density->n; k++) {
      EXPECT_EQ(test_density->values[k], density->values[k]);
      if (test_density->values[k] != density->values[k]) {
        GTEST_FAIL();
      }
    }
  }

  // compare pair potential
  for (int i = 0; i < ELE_SIZE; i++) {
    for (int j = 0; j <= i; j++) {
      auto pair = _pot->eam_pot_loader->loadEamPhi(i, j);
      auto test_pair = test_pot->eam_pot_loader->loadEamPhi(i, j);
      EXPECT_EQ(test_pair->n, pair->n);
      EXPECT_EQ(test_pair->invDx, pair->invDx);
      //      EXPECT_EQ(test_pair->x0, pair->x0);
      //      EXPECT_EQ(test_pair->max_val, pair->max_val);
      for (int k = 0; k < pair->n; k++) {
        EXPECT_EQ(test_pair->values[k], pair->values[k]);
        if (test_pair->values[k] != pair->values[k]) {
          GTEST_FAIL();
        }
      }
    }
  }
}

TEST_F(EamPotFsTestFixture, test_eam_fs_setfl_parser) {
  std::stringstream s;
  //  std::ofstream s("out.tmp.txt"); // can also dump to file.
  EamPotFileHeader header = {.nRho = DATA_SIZE, .nR = DATA_SIZE, .dRho = DELTA, .dR = DELTA, .cutoff = 0.5};
  dump_to_buffer(EAM_STYLE_FS, ELE_SIZE, header, _pot, s);

  constexpr int _ELE_SIZE = ELE_SIZE;
  // new parser
  SetflParser *parser = new SetflParser(s);
  // parse header
  parser->parseHeader();
  EXPECT_EQ(parser->getEles(), _ELE_SIZE);
  auto test_header = parser->getHeader();
  EXPECT_EQ(test_header.cutoff, header.cutoff);
  EXPECT_EQ(test_header.dR, header.dR);
  EXPECT_EQ(test_header.dRho, header.dRho);
  EXPECT_EQ(test_header.nR, header.nR);
  EXPECT_EQ(test_header.nRho, header.nRho);

  // parse body
  auto test_pot = eam::newInstance(EAM_STYLE_FS, parser->getEles(), 0, 0, MPI_COMM_WORLD);
  parser->parseBody(test_pot);

  // compare body
  for (int i = 0; i < ELE_SIZE; i++) {
    // compare embedded energy
    auto emb = _pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(test_pot->eam_pot_loader, nullptr);
    auto test_emb = test_pot->eam_pot_loader->loadEmbedded(i);
    EXPECT_NE(test_emb, nullptr);
    EXPECT_EQ(test_emb->n, emb->n);
    EXPECT_EQ(test_emb->invDx, emb->invDx);
    // todo: x0 and max_val are generated in a different way in the fixture. we currently ignore the tests.
    // EXPECT_EQ(test_emb->x0, emb->x0);
    // EXPECT_EQ(test_emb->max_val, emb->max_val);
    for (int k = 0; k < emb->n; k++) {
      EXPECT_EQ(test_emb->values[k], emb->values[k]);
      if (test_emb->values[k] != emb->values[k]) {
        GTEST_FAIL();
      }
    }

    // compare electron density
    for (int j = 0; j < ELE_SIZE; j++) {
      auto density = _pot->eam_pot_loader->loadElectronDensity(j, i);
      auto test_density = test_pot->eam_pot_loader->loadElectronDensity(j, i);
      EXPECT_EQ(test_density->n, density->n);
      EXPECT_EQ(test_density->invDx, density->invDx);
      // EXPECT_EQ(test_density->x0, density->x0);
      // EXPECT_EQ(test_density->max_val, density->max_val);
      for (int k = 0; k < density->n; k++) {
        EXPECT_EQ(test_density->values[k], density->values[k]);
        if (test_density->values[k] != density->values[k]) {
          GTEST_FAIL();
        }
      }
    }
  }

  // compare pair potential
  for (int i = 0; i < ELE_SIZE; i++) {
    for (int j = 0; j <= i; j++) {
      auto pair = _pot->eam_pot_loader->loadEamPhi(i, j);
      auto test_pair = test_pot->eam_pot_loader->loadEamPhi(i, j);
      EXPECT_EQ(test_pair->n, pair->n);
      EXPECT_EQ(test_pair->invDx, pair->invDx);
      // EXPECT_EQ(test_pair->x0, pair->x0);
      // EXPECT_EQ(test_pair->max_val, pair->max_val);
      for (int k = 0; k < pair->n; k++) {
        EXPECT_EQ(test_pair->values[k], pair->values[k]);
        if (test_pair->values[k] != pair->values[k]) {
          GTEST_FAIL();
        }
      }
    }
  }
}
